#include "vulkan-context.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>

// ============================================================================
// Вспомогательные функции работы с памятью и командами
// ============================================================================

/**
 * Поиск подходящего типа физической памяти GPU по маске типов и требуемым флагам свойств.
 *
 * @param physical_device Физическое устройство (видеокарта).
 * @param type_filter     Битовая маска допустимых типов памяти, возвращённая Vulkan.
 * @param properties      Требуемые свойства (например, DEVICE_LOCAL для видеопамяти
 *                        или HOST_VISIBLE | HOST_COHERENT для памяти, доступной ЦП).
 */
static uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);

    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        // Проверяем, разрешён ли данный тип памяти оборудованием и содержит ли он все нужные свойства
        if ((type_filter & (1 << i)) && (mem_props.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}

/**
 * Процедурное создание буфера Vulkan и выделение под него памяти.
 */
static bool create_buffer(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size,
                          VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer* buffer, VkDeviceMemory* memory) {
    VkBufferCreateInfo buffer_info = {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .size                  = size,
        // Назначение буфера: вершинный, индексный или промежуточный (staging)
        .usage                 = usage,
        // EXCLUSIVE: буфер используется только одной очередью (Graphics), что даёт максимальную производительность
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr
    };

    if (vkCreateBuffer(device, &buffer_info, nullptr, buffer) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, *buffer, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = find_memory_type(physical_device, mem_reqs.memoryTypeBits, properties)
    };

    if (vkAllocateMemory(device, &alloc_info, nullptr, memory) != VK_SUCCESS) {
        return false;
    }

    // Привязываем выделенную память к объекту буфера со смещением 0
    vkBindBufferMemory(device, *buffer, *memory, 0);
    return true;
}

/**
 * Начало записи одноразового первичного командного буфера (для операций загрузки текстур на GPU).
 */
static VkCommandBuffer begin_single_time_commands(VulkanContext* ctx) {
    VkCommandBufferAllocateInfo alloc_info = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = ctx->frames[0].command_pool,
        // PRIMARY: командный буфер отправляется напрямую в очередь GPU без вложения
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer cmd = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(ctx->device, &alloc_info, &cmd);

    VkCommandBufferBeginInfo begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        // ONE_TIME_SUBMIT: буфер будет выполнен ровно один раз и сразу освобождён
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };

    vkBeginCommandBuffer(cmd, &begin_info);
    return cmd;
}

/**
 * Завершение записи, отправка одноразового командного буфера в очередь и синхронное ожидание GPU.
 */
static void end_single_time_commands(VulkanContext* ctx, VkCommandBuffer cmd) {
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submit_info = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores    = nullptr
    };

    vkQueueSubmit(ctx->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(ctx->graphics_queue);

    vkFreeCommandBuffers(ctx->device, ctx->frames[0].command_pool, 1, &cmd);
}

/**
 * Перевод раскладки памяти текстуры (VkImage Layout Transition) через барьер памяти конвейера.
 * Необходим для корректной передачи растрированных страниц PDF от ЦП в шейдерный сэмплер.
 */
static void transition_image_layout(VulkanContext* ctx, VkImage image,
                                    VkImageLayout old_layout, VkImageLayout new_layout) {
    VkCommandBuffer cmd = begin_single_time_commands(ctx);

    VkImageMemoryBarrier barrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = 0, // Заполняется ниже в зависимости от стадий перехода
        .dstAccessMask       = 0,
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        // Очередь не меняется (EXCLUSIVE)
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image,
        .subresourceRange    = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1
        }
    };

    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Подготовка текстуры к приёму пикселей страницы PDF из staging-буфера
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage             = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage             = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Перевод текстуры в состояние чтения фрагментным шейдером после окончания копирования
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage             = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage             = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Перевод для обновления существующей текстуры новыми пикселями (при смене страницы)
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage             = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dst_stage             = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        src_stage             = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage             = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }

    vkCmdPipelineBarrier(cmd, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    end_single_time_commands(ctx, cmd);
}

// ============================================================================
// Загрузка скомпилированных GLSL-шейдеров (SPIR-V)
// ============================================================================

static std::vector<char> read_shader_file_spv(const std::string& filename) {
    std::vector<std::string> search_paths;

#ifdef SHADERS_DIR
    search_paths.push_back(std::string(SHADERS_DIR) + "/" + filename);
#endif
    search_paths.push_back("shaders/" + filename);
    search_paths.push_back(filename);

    for (const auto& path : search_paths) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (file.is_open()) {
            size_t file_size = static_cast<size_t>(file.tellg());
            std::vector<char> buffer(file_size);
            file.seekg(0);
            file.read(buffer.data(), file_size);
            file.close();
            return buffer;
        }
    }

    std::cerr << "[VulkanContext] Error: Unable to locate or open compiled shader: " << filename << "\n";
    return {};
}

static VkShaderModule create_shader_module_from_spv(VkDevice device, const std::string& filename) {
    std::vector<char> code = read_shader_file_spv(filename);
    if (code.empty()) {
        return VK_NULL_HANDLE;
    }

    VkShaderModuleCreateInfo create_info = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        // Размер бинарного кода в байтах
        .codeSize = code.size(),
        // Указатель на массив 32-битных инструкций SPIR-V
        .pCode    = reinterpret_cast<const uint32_t*>(code.data())
    };

    VkShaderModule shader = VK_NULL_HANDLE;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader) != VK_SUCCESS) {
        std::cerr << "[VulkanContext] Failed to create shader module from: " << filename << "\n";
        return VK_NULL_HANDLE;
    }
    return shader;
}

// ============================================================================
// Инициализация Vulkan: Instance, GPU, Logical Device
// ============================================================================

static bool create_instance(VulkanContext* ctx, const VulkanConfig* config) {
    VkApplicationInfo app_info = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = "New Horizons PDF & Narration Viewer",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "HorizonEngine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        // Vulkan 1.2 предоставляет стабильную поддержку отрицательных Viewport и современных расширений
        .apiVersion         = VK_API_VERSION_1_2
    };

    uint32_t ext_count = 0;
    const char* const* window_extensions = window_get_required_instance_extensions(&ext_count);

    std::vector<const char*> extensions(window_extensions, window_extensions + ext_count);

    std::vector<const char*> layers;
    if (config && config->enable_validation) {
        // Стандартный валидационный слой Khronos для отлова некорректных вызовов API в отладке
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    VkInstanceCreateInfo create_info = {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .pApplicationInfo        = &app_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        // Расширения для работы с окном: VK_KHR_surface и VK_KHR_win32_surface
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    VkResult res = vkCreateInstance(&create_info, nullptr, &ctx->instance);
    if (res != VK_SUCCESS && !layers.empty()) {
        std::cerr << "[Vulkan] Validation layers unavailable, retrying without validation.\n";
        create_info.enabledLayerCount   = 0;
        create_info.ppEnabledLayerNames = nullptr;
        res = vkCreateInstance(&create_info, nullptr, &ctx->instance);
    }

    return res == VK_SUCCESS;
}

static bool pick_physical_device(VulkanContext* ctx) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(ctx->instance, &device_count, nullptr);
    if (device_count == 0) return false;

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(ctx->instance, &device_count, devices.data());

    for (VkPhysicalDevice dev : devices) {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, queue_families.data());

        int graphics_family = -1;
        int present_family  = -1;

        for (uint32_t i = 0; i < queue_family_count; i++) {
            // Ищем семейство очередей с поддержкой графических команд
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphics_family = i;
            }

            // И семейство с возможностью вывода изображения в окно Win32 Surface
            VkBool32 present_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, ctx->surface, &present_support);
            if (present_support) {
                present_family = i;
            }

            if (graphics_family != -1 && present_family != -1) {
                break;
            }
        }

        if (graphics_family != -1 && present_family != -1) {
            ctx->physical_device       = dev;
            ctx->graphics_queue_family = static_cast<uint32_t>(graphics_family);
            ctx->present_queue_family  = static_cast<uint32_t>(present_family);
            return true;
        }
    }

    return false;
}

static bool create_logical_device(VulkanContext* ctx) {
    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    std::vector<uint32_t> unique_families = { ctx->graphics_queue_family };
    if (ctx->present_queue_family != ctx->graphics_queue_family) {
        unique_families.push_back(ctx->present_queue_family);
    }

    float priority = 1.0f;
    for (uint32_t family : unique_families) {
        VkDeviceQueueCreateInfo qinfo = {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .queueFamilyIndex = family,
            .queueCount       = 1,
            .pQueuePriorities = &priority
        };
        queue_infos.push_back(qinfo);
    }

    // Для 2D рендеринга страниц PDF анизотропная фильтрация не требуется (используем билинейную)
    VkPhysicalDeviceFeatures device_features = {
        .samplerAnisotropy = VK_FALSE
    };

    // Обязательное расширение для вывода на экран (Swapchain)
    const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo create_info = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .queueCreateInfoCount    = static_cast<uint32_t>(queue_infos.size()),
        .pQueueCreateInfos       = queue_infos.data(),
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nullptr,
        .enabledExtensionCount   = 1,
        .ppEnabledExtensionNames = device_extensions,
        .pEnabledFeatures        = &device_features
    };

    if (vkCreateDevice(ctx->physical_device, &create_info, nullptr, &ctx->device) != VK_SUCCESS) {
        return false;
    }

    vkGetDeviceQueue(ctx->device, ctx->graphics_queue_family, 0, &ctx->graphics_queue);
    vkGetDeviceQueue(ctx->device, ctx->present_queue_family, 0, &ctx->present_queue);
    return true;
}

// ============================================================================
// Настройка цепочки показа (Swapchain)
// ============================================================================

static bool create_swapchain_resources(VulkanContext* ctx) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->physical_device, ctx->surface, &capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physical_device, ctx->surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physical_device, ctx->surface, &format_count, formats.data());

    // Выбираем стандартный 32-битный цвет B8G8R8A8 или R8G8B8A8 в нелинейном sRGB
    VkSurfaceFormatKHR surface_format = formats[0];
    for (const auto& fmt : formats) {
        if ((fmt.format == VK_FORMAT_B8G8R8A8_UNORM || fmt.format == VK_FORMAT_R8G8B8A8_UNORM) &&
            fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surface_format = fmt;
            break;
        }
    }
    ctx->swapchain_format = surface_format.format;

    uint32_t width = 0, height = 0;
    window_get_framebuffer_size(ctx->window, &width, &height);

    if (capabilities.currentExtent.width != UINT32_MAX) {
        ctx->swapchain_extent = capabilities.currentExtent;
    } else {
        ctx->swapchain_extent.width  = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        ctx->swapchain_extent.height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    if (ctx->swapchain_extent.width == 0 || ctx->swapchain_extent.height == 0) {
        return false; // Окно свёрнуто
    }

    // Тройная/двойная буферизация: берём на 1 изображение больше минимума, чтобы не ждать освобождения драйвером
    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    uint32_t queue_indices[] = { ctx->graphics_queue_family, ctx->present_queue_family };
    bool separate_queues     = (ctx->graphics_queue_family != ctx->present_queue_family);

    VkSwapchainCreateInfoKHR create_info = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0,
        .surface               = ctx->surface,
        .minImageCount         = image_count,
        .imageFormat           = surface_format.format,
        .imageColorSpace       = surface_format.colorSpace,
        .imageExtent           = ctx->swapchain_extent,
        .imageArrayLayers      = 1,
        // COLOR_ATTACHMENT: кадры рендерятся прямо в буфер экрана
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        // CONCURRENT, если графическая очередь и очередь вывода разные; иначе EXCLUSIVE
        .imageSharingMode      = separate_queues ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = separate_queues ? 2u : 0u,
        .pQueueFamilyIndices   = separate_queues ? queue_indices : nullptr,
        // Трансформации не требуются (в отличие от мобильных устройств с поворотом экрана)
        .preTransform          = capabilities.currentTransform,
        // OPAQUE: само окно непрозрачно относительно рабочего стола Windows
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        // FIFO: гарантированная вертикальная синхронизация без разрывов кадров (tearing)
        .presentMode           = VK_PRESENT_MODE_FIFO_KHR,
        // clipped = TRUE: отсекаем отрисовку пикселей, перекрытых другими окнами ОС
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE
    };

    if (vkCreateSwapchainKHR(ctx->device, &create_info, nullptr, &ctx->swapchain) != VK_SUCCESS) {
        return false;
    }

    vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->swapchain_image_count, nullptr);
    ctx->swapchain_images = new VkImage[ctx->swapchain_image_count];
    vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->swapchain_image_count, ctx->swapchain_images);

    ctx->swapchain_image_views = new VkImageView[ctx->swapchain_image_count];
    for (uint32_t i = 0; i < ctx->swapchain_image_count; i++) {
        VkImageViewCreateInfo view_info = {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .image            = ctx->swapchain_images[i],
            .viewType         = VK_IMAGE_VIEW_TYPE_2D,
            .format           = ctx->swapchain_format,
            .components       = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1
            }
        };

        if (vkCreateImageView(ctx->device, &view_info, nullptr, &ctx->swapchain_image_views[i]) != VK_SUCCESS) {
            return false;
        }
    }

    return true;
}

// ============================================================================
// Создание RenderPass и Framebuffers
// ============================================================================

static bool create_render_pass(VulkanContext* ctx) {
    // Описание цветового вложения кадра
    VkAttachmentDescription color_attachment = {
        .flags          = 0,
        .format         = ctx->swapchain_format,
        .samples        = VK_SAMPLE_COUNT_1_BIT, // Без мультисэмплинга, пиксели 1:1
        // CLEAR: очистка экрана цветом фона в начале каждого кадра
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        // STORE: сохранение результата отрисовки для отправки на дисплей
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        // UNDEFINED: нам не важно предыдущее содержимое буфера, так как делаем полную очистку
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        // PRESENT_SRC: в конце прохода изображение автоматически переводится в формат показа
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference color_ref = {
        .attachment = 0,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_ref,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = nullptr
    };

    // Зависимость подпрохода: ожидаем готовности буфера цвета перед началом записи
    VkSubpassDependency dependency = {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask   = 0,
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0
    };

    VkRenderPassCreateInfo render_pass_info = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .attachmentCount = 1,
        .pAttachments    = &color_attachment,
        .subpassCount    = 1,
        .pSubpasses      = &subpass,
        .dependencyCount = 1,
        .pDependencies   = &dependency
    };

    return vkCreateRenderPass(ctx->device, &render_pass_info, nullptr, &ctx->render_pass) == VK_SUCCESS;
}

static bool create_framebuffers(VulkanContext* ctx) {
    ctx->swapchain_framebuffers = new VkFramebuffer[ctx->swapchain_image_count];

    for (uint32_t i = 0; i < ctx->swapchain_image_count; i++) {
        VkImageView attachments[] = { ctx->swapchain_image_views[i] };

        VkFramebufferCreateInfo fb_info = {
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .renderPass      = ctx->render_pass,
            .attachmentCount = 1,
            .pAttachments    = attachments,
            .width           = ctx->swapchain_extent.width,
            .height          = ctx->swapchain_extent.height,
            .layers          = 1
        };

        if (vkCreateFramebuffer(ctx->device, &fb_info, nullptr, &ctx->swapchain_framebuffers[i]) != VK_SUCCESS) {
            return false;
        }
    }
    return true;
}

// ============================================================================
// Создание графического пайплайна 2D: Дескрипторы, Блендинг, Сэмплер
// ============================================================================

static bool create_pipeline_2d(VulkanContext* ctx) {
    // 1. Привязка дескриптора текстурного сэмплера (binding = 0 во фрагментном шейдере)
    VkDescriptorSetLayoutBinding sampler_binding = {
        .binding            = 0,
        .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount    = 1,
        .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0,
        .bindingCount = 1,
        .pBindings    = &sampler_binding
    };

    if (vkCreateDescriptorSetLayout(ctx->device, &layout_info, nullptr, &ctx->desc_layout) != VK_SUCCESS) {
        return false;
    }

    // Пул дескрипторов: позволяет аллоцировать до 128 текстур (страницы документов, иконки, буферы)
    VkDescriptorPoolSize pool_size = {
        .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 128
    };

    VkDescriptorPoolCreateInfo pool_info = {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        // FREE_DESCRIPTOR_SET_BIT: позволяет освобождать отдельные дескрипторы при закрытии файлов
        .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets       = 128,
        .poolSizeCount = 1,
        .pPoolSizes    = &pool_size
    };

    if (vkCreateDescriptorPool(ctx->device, &pool_info, nullptr, &ctx->desc_pool) != VK_SUCCESS) {
        return false;
    }

    // 2. Сэмплер: линейная фильтрация для плавного масштабирования текста страниц PDF
    VkSamplerCreateInfo sampler_info = {
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .magFilter               = VK_FILTER_LINEAR, // Билинейное увеличение
        .minFilter               = VK_FILTER_LINEAR, // Билинейное уменьшение
        .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        // CLAMP_TO_EDGE: предотвращает появление артефактов на границах страниц при скролле
        .addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = VK_FALSE,
        .maxAnisotropy           = 1.0f,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 0.0f,
        .maxLod                  = 0.0f,
        .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE // Стандартные UV координаты от 0.0 до 1.0
    };

    if (vkCreateSampler(ctx->device, &sampler_info, nullptr, &ctx->default_sampler) != VK_SUCCESS) {
        return false;
    }

    // 3. Загрузка шейдеров, скомпилированных glslc во время сборки CMake
    VkShaderModule vert_shader = create_shader_module_from_spv(ctx->device, "shader_2d.vert.spv");
    VkShaderModule frag_shader = create_shader_module_from_spv(ctx->device, "shader_2d.frag.spv");
    if (!vert_shader || !frag_shader) {
        std::cerr << "[VulkanContext] Failed to load SPIR-V shader modules from disk!\n";
        return false;
    }

    VkPipelineShaderStageCreateInfo shader_stages[2] = {
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = VK_SHADER_STAGE_VERTEX_BIT,
            .module              = vert_shader,
            .pName               = "main",
            .pSpecializationInfo = nullptr
        },
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module              = frag_shader,
            .pName               = "main",
            .pSpecializationInfo = nullptr
        }
    };

    // 4. Описание структуры вершины Vertex2D
    VkVertexInputBindingDescription binding_desc = {
        .binding   = 0,
        .stride    = sizeof(Vertex2D),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription attr_descs[3] = {
        {
            // location 0: pos (vec2: x, y в пикселях экрана)
            .location = 0,
            .binding  = 0,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(Vertex2D, pos)
        },
        {
            // location 1: uv (vec2: текстурные координаты)
            .location = 1,
            .binding  = 0,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(Vertex2D, uv)
        },
        {
            // location 2: color (vec4: цвет RGBA для подсветки и заливки)
            .location = 2,
            .binding  = 0,
            .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset   = offsetof(Vertex2D, color)
        }
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0,
        .vertexBindingDescriptionCount   = 1,
        .pVertexBindingDescriptions      = &binding_desc,
        .vertexAttributeDescriptionCount = 3,
        .pVertexAttributeDescriptions    = attr_descs
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Рисуем прямоугольники парами треугольников
        .primitiveRestartEnable = VK_FALSE
    };

    // Viewport и Scissor динамические (настраиваются при каждом изменении размера окна)
    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = nullptr,
        .scissorCount  = 1,
        .pScissors     = nullptr
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        // CULL_NONE: отключаем отсечение граней, так как 2D-прямоугольники плоские
        .cullMode                = VK_CULL_MODE_NONE,
        .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE
    };

    // 5. Альфа-блендинг (Ключевая часть для подсветки текста голосовой моделью):
    // Формула: Result = SourceColor * SourceAlpha + DestColor * (1 - SourceAlpha)
    // Позволяет мягко накладывать жёлтые, зелёные или синие маркеры поверх чёрного текста книги/PDF
    VkPipelineColorBlendAttachmentState color_blend = {
        .blendEnable         = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                               VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo blend_state = {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &color_blend,
        .blendConstants  = { 0.0f, 0.0f, 0.0f, 0.0f }
    };

    // Push Constants: передача размеров окна в вершинный шейдер без обращения к Uniform-буферам
    VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset     = 0,
        .size       = sizeof(float) * 4 // vec2 uScale + vec2 uTranslate
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = 1,
        .pSetLayouts            = &ctx->desc_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range
    };

    if (vkCreatePipelineLayout(ctx->device, &pipeline_layout_info, nullptr, &ctx->pipeline_layout) != VK_SUCCESS) {
        return false;
    }

    VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0,
        .dynamicStateCount = 2,
        .pDynamicStates    = dynamic_states
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stageCount          = 2,
        .pStages             = shader_stages,
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &blend_state,
        .pDynamicState       = &dynamic_state_info,
        .layout              = ctx->pipeline_layout,
        .renderPass          = ctx->render_pass,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1
    };

    VkResult res = vkCreateGraphicsPipelines(ctx->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &ctx->pipeline_2d);

    vkDestroyShaderModule(ctx->device, vert_shader, nullptr);
    vkDestroyShaderModule(ctx->device, frag_shader, nullptr);

    return res == VK_SUCCESS;
}

// ============================================================================
// Создание динамических буферов и синхронизации для кадров в полёте
// ============================================================================

static bool create_frames_data(VulkanContext* ctx) {
    for (uint32_t i = 0; i < VULKAN_MAX_FRAMES_IN_FLIGHT; i++) {
        FrameData& frame = ctx->frames[i];

        VkCommandPoolCreateInfo pool_info = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = nullptr,
            // RESET_COMMAND_BUFFER: позволяет многократно перезаписывать буфер каждый кадр
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = ctx->graphics_queue_family
        };

        if (vkCreateCommandPool(ctx->device, &pool_info, nullptr, &frame.command_pool) != VK_SUCCESS) {
            return false;
        }

        VkCommandBufferAllocateInfo alloc_info = {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = frame.command_pool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if (vkAllocateCommandBuffers(ctx->device, &alloc_info, &frame.command_buffer) != VK_SUCCESS) {
            return false;
        }

        VkSemaphoreCreateInfo sem_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        VkFenceCreateInfo fence_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            // SIGNALED: забор изначально поднят, чтобы первый кадр не блокировался вызовом waitForFences
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        if (vkCreateSemaphore(ctx->device, &sem_info, nullptr, &frame.image_available_sem) != VK_SUCCESS ||
            vkCreateSemaphore(ctx->device, &sem_info, nullptr, &frame.render_finished_sem) != VK_SUCCESS ||
            vkCreateFence(ctx->device, &fence_info, nullptr, &frame.in_flight_fence) != VK_SUCCESS) {
            return false;
        }

        // Выделяем память под динамические вершины: HOST_VISIBLE | HOST_COHERENT позволяет писать прямо через ЦП
        VkDeviceSize vbuf_size = VULKAN_MAX_2D_VERTS * sizeof(Vertex2D);
        if (!create_buffer(ctx->device, ctx->physical_device, vbuf_size,
                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          &frame.vertex_buffer, &frame.vertex_memory)) {
            return false;
        }
        vkMapMemory(ctx->device, frame.vertex_memory, 0, vbuf_size, 0, reinterpret_cast<void**>(&frame.mapped_vertices));

        // Выделяем память под индексы треугольников (каждый quad = 6 индексов)
        VkDeviceSize ibuf_size = VULKAN_MAX_2D_INDICES * sizeof(uint32_t);
        if (!create_buffer(ctx->device, ctx->physical_device, ibuf_size,
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          &frame.index_buffer, &frame.index_memory)) {
            return false;
        }
        vkMapMemory(ctx->device, frame.index_memory, 0, ibuf_size, 0, reinterpret_cast<void**>(&frame.mapped_indices));
    }

    return true;
}

// ============================================================================
// Публичный API: Инициализация и Жизненный цикл
// ============================================================================

bool vulkan_context_init(VulkanContext* ctx, Window* window, const VulkanConfig* config) {
    if (!ctx || !window) return false;
    ctx->window = window;

    if (!create_instance(ctx, config)) {
        std::cerr << "[VulkanContext] Failed to create Vulkan instance!\n";
        return false;
    }

    if (window_create_vulkan_surface(window, ctx->instance, nullptr, &ctx->surface) != VK_SUCCESS) {
        std::cerr << "[VulkanContext] Failed to create Vulkan surface from window!\n";
        return false;
    }

    if (!pick_physical_device(ctx)) {
        std::cerr << "[VulkanContext] Failed to find suitable GPU!\n";
        return false;
    }

    if (!create_logical_device(ctx)) {
        std::cerr << "[VulkanContext] Failed to create logical device!\n";
        return false;
    }

    if (!create_swapchain_resources(ctx)) {
        std::cerr << "[VulkanContext] Failed to create swapchain!\n";
        return false;
    }

    if (!create_render_pass(ctx)) {
        std::cerr << "[VulkanContext] Failed to create render pass!\n";
        return false;
    }

    if (!create_framebuffers(ctx)) {
        std::cerr << "[VulkanContext] Failed to create framebuffers!\n";
        return false;
    }

    if (!create_pipeline_2d(ctx)) {
        std::cerr << "[VulkanContext] Failed to create 2D graphics pipeline!\n";
        return false;
    }

    if (!create_frames_data(ctx)) {
        std::cerr << "[VulkanContext] Failed to create frame data buffers!\n";
        return false;
    }

    // Создаём белую текстуру 1x1 для сплошных цветов и подсветок
    uint32_t white_pixel = 0xFFFFFFFF;
    if (!vulkan_create_texture(ctx, 1, 1, &white_pixel, &ctx->white_texture)) {
        std::cerr << "[VulkanContext] Failed to create default 1x1 white texture!\n";
        return false;
    }

    return true;
}

void vulkan_context_cleanup(VulkanContext* ctx) {
    if (!ctx || !ctx->device) return;

    vkDeviceWaitIdle(ctx->device);

    vulkan_destroy_texture(ctx, &ctx->white_texture);

    for (uint32_t i = 0; i < VULKAN_MAX_FRAMES_IN_FLIGHT; i++) {
        FrameData& frame = ctx->frames[i];
        if (frame.vertex_memory) {
            vkUnmapMemory(ctx->device, frame.vertex_memory);
            vkFreeMemory(ctx->device, frame.vertex_memory, nullptr);
        }
        if (frame.vertex_buffer) vkDestroyBuffer(ctx->device, frame.vertex_buffer, nullptr);

        if (frame.index_memory) {
            vkUnmapMemory(ctx->device, frame.index_memory);
            vkFreeMemory(ctx->device, frame.index_memory, nullptr);
        }
        if (frame.index_buffer) vkDestroyBuffer(ctx->device, frame.index_buffer, nullptr);

        if (frame.image_available_sem) vkDestroySemaphore(ctx->device, frame.image_available_sem, nullptr);
        if (frame.render_finished_sem) vkDestroySemaphore(ctx->device, frame.render_finished_sem, nullptr);
        if (frame.in_flight_fence) vkDestroyFence(ctx->device, frame.in_flight_fence, nullptr);

        if (frame.command_pool) vkDestroyCommandPool(ctx->device, frame.command_pool, nullptr);
    }

    if (ctx->pipeline_2d) vkDestroyPipeline(ctx->device, ctx->pipeline_2d, nullptr);
    if (ctx->pipeline_layout) vkDestroyPipelineLayout(ctx->device, ctx->pipeline_layout, nullptr);
    if (ctx->default_sampler) vkDestroySampler(ctx->device, ctx->default_sampler, nullptr);
    if (ctx->desc_pool) vkDestroyDescriptorPool(ctx->device, ctx->desc_pool, nullptr);
    if (ctx->desc_layout) vkDestroyDescriptorSetLayout(ctx->device, ctx->desc_layout, nullptr);

    for (uint32_t i = 0; i < ctx->swapchain_image_count; i++) {
        if (ctx->swapchain_framebuffers) vkDestroyFramebuffer(ctx->device, ctx->swapchain_framebuffers[i], nullptr);
        if (ctx->swapchain_image_views) vkDestroyImageView(ctx->device, ctx->swapchain_image_views[i], nullptr);
    }
    delete[] ctx->swapchain_framebuffers;
    delete[] ctx->swapchain_image_views;
    delete[] ctx->swapchain_images;

    if (ctx->render_pass) vkDestroyRenderPass(ctx->device, ctx->render_pass, nullptr);
    if (ctx->swapchain) vkDestroySwapchainKHR(ctx->device, ctx->swapchain, nullptr);

    vkDestroyDevice(ctx->device, nullptr);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, nullptr);
    vkDestroyInstance(ctx->instance, nullptr);

    *ctx = VulkanContext{};
}

bool vulkan_context_recreate_swapchain(VulkanContext* ctx) {
    if (!ctx || !ctx->window) return false;

    uint32_t width = 0, height = 0;
    window_get_framebuffer_size(ctx->window, &width, &height);
    if (width == 0 || height == 0) {
        return false; // Окно свёрнуто
    }

    vkDeviceWaitIdle(ctx->device);

    for (uint32_t i = 0; i < ctx->swapchain_image_count; i++) {
        vkDestroyFramebuffer(ctx->device, ctx->swapchain_framebuffers[i], nullptr);
        vkDestroyImageView(ctx->device, ctx->swapchain_image_views[i], nullptr);
    }
    delete[] ctx->swapchain_framebuffers;
    delete[] ctx->swapchain_image_views;
    delete[] ctx->swapchain_images;
    ctx->swapchain_framebuffers = nullptr;
    ctx->swapchain_image_views  = nullptr;
    ctx->swapchain_images       = nullptr;

    vkDestroySwapchainKHR(ctx->device, ctx->swapchain, nullptr);

    if (!create_swapchain_resources(ctx)) return false;
    if (!create_framebuffers(ctx)) return false;

    return true;
}

// ============================================================================
// Управление текстурами страниц документов (PDF)
// ============================================================================

bool vulkan_create_texture(VulkanContext* ctx, uint32_t width, uint32_t height, const void* rgba_pixels, VulkanTexture* out_texture) {
    if (!ctx || !out_texture || width == 0 || height == 0) return false;

    VkDeviceSize image_size = width * height * 4;

    VkBuffer staging_buffer       = VK_NULL_HANDLE;
    VkDeviceMemory staging_memory = VK_NULL_HANDLE;

    // Выделяем промежуточный хост-буфер для отправки растрированных пикселей страницы на GPU
    if (!create_buffer(ctx->device, ctx->physical_device, image_size,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       &staging_buffer, &staging_memory)) {
        return false;
    }

    void* data = nullptr;
    vkMapMemory(ctx->device, staging_memory, 0, image_size, 0, &data);
    if (rgba_pixels) {
        std::memcpy(data, rgba_pixels, static_cast<size_t>(image_size));
    } else {
        std::memset(data, 0xFF, static_cast<size_t>(image_size));
    }
    vkUnmapMemory(ctx->device, staging_memory);

    // Создаём текстуру VkImage в оптимальной локальной памяти видеокарты
    VkImageCreateInfo image_info = {
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .imageType             = VK_IMAGE_TYPE_2D,
        .format                = VK_FORMAT_R8G8B8A8_UNORM, // 8 бит на канал RGBA
        .extent                = {
            .width  = width,
            .height = height,
            .depth  = 1
        },
        .mipLevels             = 1,
        .arrayLayers           = 1,
        .samples               = VK_SAMPLE_COUNT_1_BIT,
        // TILING_OPTIMAL: внутренний порядок пикселей оптимизирован под видеопамять конкретного GPU
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        // TRANSFER_DST (приёмник пикселей) + SAMPLED (чтение в шейдере)
        .usage                 = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
    };

    if (vkCreateImage(ctx->device, &image_info, nullptr, &out_texture->image) != VK_SUCCESS) {
        vkDestroyBuffer(ctx->device, staging_buffer, nullptr);
        vkFreeMemory(ctx->device, staging_memory, nullptr);
        return false;
    }

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(ctx->device, out_texture->image, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = mem_reqs.size,
        // DEVICE_LOCAL: быстрая видеопамять GPU
        .memoryTypeIndex = find_memory_type(ctx->physical_device, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };

    if (vkAllocateMemory(ctx->device, &alloc_info, nullptr, &out_texture->memory) != VK_SUCCESS) {
        return false;
    }
    vkBindImageMemory(ctx->device, out_texture->image, out_texture->memory, 0);

    // Переход: UNDEFINED -> TRANSFER_DST_OPTIMAL
    transition_image_layout(ctx, out_texture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Копирование пикселей из промежуточного буфера в видеопамять текстуры
    VkCommandBuffer cmd = begin_single_time_commands(ctx);
    VkBufferImageCopy region = {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource  = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel       = 0,
            .baseArrayLayer = 0,
            .layerCount     = 1
        },
        .imageOffset       = { 0, 0, 0 },
        .imageExtent       = { width, height, 1 }
    };
    vkCmdCopyBufferToImage(cmd, staging_buffer, out_texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    end_single_time_commands(ctx, cmd);

    // Переход: TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
    transition_image_layout(ctx, out_texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(ctx->device, staging_buffer, nullptr);
    vkFreeMemory(ctx->device, staging_memory, nullptr);

    // Создание ImageView для доступа к изображению из шейдеров
    VkImageViewCreateInfo view_info = {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .image            = out_texture->image,
        .viewType         = VK_IMAGE_VIEW_TYPE_2D,
        .format           = VK_FORMAT_R8G8B8A8_UNORM,
        .components       = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1
        }
    };

    if (vkCreateImageView(ctx->device, &view_info, nullptr, &out_texture->view) != VK_SUCCESS) {
        return false;
    }

    // Выделение и привязка дескрипторного набора текстуры к сэмплеру
    VkDescriptorSetAllocateInfo ds_alloc = {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = ctx->desc_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &ctx->desc_layout
    };

    if (vkAllocateDescriptorSets(ctx->device, &ds_alloc, &out_texture->desc_set) != VK_SUCCESS) {
        return false;
    }

    VkDescriptorImageInfo image_desc = {
        .sampler     = ctx->default_sampler,
        .imageView   = out_texture->view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkWriteDescriptorSet write_ds = {
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = out_texture->desc_set,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = &image_desc,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr
    };

    vkUpdateDescriptorSets(ctx->device, 1, &write_ds, 0, nullptr);

    out_texture->width  = width;
    out_texture->height = height;
    return true;
}

bool vulkan_update_texture(VulkanContext* ctx, VulkanTexture* texture, uint32_t width, uint32_t height, const void* rgba_pixels) {
    if (!ctx || !texture || !rgba_pixels || width == 0 || height == 0) return false;

    // Если разрешение изменилось (например, при зуме PDF) — пересоздаём текстуру
    if (texture->width != width || texture->height != height) {
        vulkan_destroy_texture(ctx, texture);
        return vulkan_create_texture(ctx, width, height, rgba_pixels, texture);
    }

    VkDeviceSize image_size = width * height * 4;
    VkBuffer staging_buffer       = VK_NULL_HANDLE;
    VkDeviceMemory staging_memory = VK_NULL_HANDLE;

    if (!create_buffer(ctx->device, ctx->physical_device, image_size,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       &staging_buffer, &staging_memory)) {
        return false;
    }

    void* data = nullptr;
    vkMapMemory(ctx->device, staging_memory, 0, image_size, 0, &data);
    std::memcpy(data, rgba_pixels, static_cast<size_t>(image_size));
    vkUnmapMemory(ctx->device, staging_memory);

    transition_image_layout(ctx, texture->image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkCommandBuffer cmd = begin_single_time_commands(ctx);
    VkBufferImageCopy region = {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource  = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel       = 0,
            .baseArrayLayer = 0,
            .layerCount     = 1
        },
        .imageOffset       = { 0, 0, 0 },
        .imageExtent       = { width, height, 1 }
    };
    vkCmdCopyBufferToImage(cmd, staging_buffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    end_single_time_commands(ctx, cmd);

    transition_image_layout(ctx, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(ctx->device, staging_buffer, nullptr);
    vkFreeMemory(ctx->device, staging_memory, nullptr);
    return true;
}

void vulkan_destroy_texture(VulkanContext* ctx, VulkanTexture* texture) {
    if (!ctx || !ctx->device || !texture) return;

    if (texture->desc_set && ctx->desc_pool) {
        vkFreeDescriptorSets(ctx->device, ctx->desc_pool, 1, &texture->desc_set);
        texture->desc_set = VK_NULL_HANDLE;
    }

    if (texture->view) {
        vkDestroyImageView(ctx->device, texture->view, nullptr);
        texture->view = VK_NULL_HANDLE;
    }

    if (texture->image) {
        vkDestroyImage(ctx->device, texture->image, nullptr);
        texture->image = VK_NULL_HANDLE;
    }

    if (texture->memory) {
        vkFreeMemory(ctx->device, texture->memory, nullptr);
        texture->memory = VK_NULL_HANDLE;
    }

    texture->width  = 0;
    texture->height = 0;
}

// ============================================================================
// Цикл рендеринга кадра
// ============================================================================

bool vulkan_begin_frame(VulkanContext* ctx, float clear_r, float clear_g, float clear_b, float clear_a) {
    if (!ctx || !ctx->window) return false;

    // Автоматическая пересборка Swapchain при изменении размера окна
    if (window_was_resized(ctx->window)) {
        if (!vulkan_context_recreate_swapchain(ctx)) {
            return false;
        }
        window_reset_resized_flag(ctx->window);
    }

    if (ctx->swapchain_extent.width == 0 || ctx->swapchain_extent.height == 0) {
        return false; // Окно свёрнуто
    }

    FrameData& frame = ctx->frames[ctx->current_frame];
    // Ждём, пока GPU завершит отрисовку предыдущего использования этого слота кадра
    vkWaitForFences(ctx->device, 1, &frame.in_flight_fence, VK_TRUE, UINT64_MAX);

    VkResult res = vkAcquireNextImageKHR(ctx->device, ctx->swapchain, UINT64_MAX,
                                         frame.image_available_sem, VK_NULL_HANDLE, &ctx->current_image_index);

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        vulkan_context_recreate_swapchain(ctx);
        return false;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        return false;
    }

    vkResetFences(ctx->device, 1, &frame.in_flight_fence);
    vkResetCommandBuffer(frame.command_buffer, 0);

    VkCommandBufferBeginInfo begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };
    vkBeginCommandBuffer(frame.command_buffer, &begin_info);

    VkClearValue clear_color = {
        .color = { .float32 = { clear_r, clear_g, clear_b, clear_a } }
    };

    VkRenderPassBeginInfo rp_info = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext           = nullptr,
        .renderPass      = ctx->render_pass,
        .framebuffer     = ctx->swapchain_framebuffers[ctx->current_image_index],
        .renderArea      = {
            .offset = { 0, 0 },
            .extent = ctx->swapchain_extent
        },
        .clearValueCount = 1,
        .pClearValues    = &clear_color
    };

    vkCmdBeginRenderPass(frame.command_buffer, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pipeline_2d);

    VkViewport viewport = {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>(ctx->swapchain_extent.width),
        .height   = static_cast<float>(ctx->swapchain_extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(frame.command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = { 0, 0 },
        .extent = ctx->swapchain_extent
    };
    vkCmdSetScissor(frame.command_buffer, 0, 1, &scissor);

    // Push Constants для ортографической проекции: экранные пиксели (0..W, 0..H) -> NDC [-1..1]
    float push_constants[4] = {
        2.0f / static_cast<float>(ctx->swapchain_extent.width),
        2.0f / static_cast<float>(ctx->swapchain_extent.height),
        -1.0f,
        -1.0f
    };
    vkCmdPushConstants(frame.command_buffer, ctx->pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(push_constants), push_constants);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(frame.command_buffer, 0, 1, &frame.vertex_buffer, offsets);
    vkCmdBindIndexBuffer(frame.command_buffer, frame.index_buffer, 0, VK_INDEX_TYPE_UINT32);

    ctx->batch_quad_count          = 0;
    ctx->batch_draw_offset_indices = 0;
    ctx->active_desc_set           = VK_NULL_HANDLE;
    ctx->is_rendering              = true;

    return true;
}

void vulkan_flush_batch(VulkanContext* ctx) {
    if (!ctx || !ctx->is_rendering || ctx->batch_quad_count == 0) return;

    FrameData& frame = ctx->frames[ctx->current_frame];

    if (ctx->active_desc_set != VK_NULL_HANDLE) {
        vkCmdBindDescriptorSets(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                ctx->pipeline_layout, 0, 1, &ctx->active_desc_set, 0, nullptr);
    }

    uint32_t index_count = ctx->batch_quad_count * 6;
    vkCmdDrawIndexed(frame.command_buffer, index_count, 1, ctx->batch_draw_offset_indices, 0, 0);

    ctx->batch_draw_offset_indices += index_count;
    ctx->batch_quad_count           = 0;
}

bool vulkan_end_frame(VulkanContext* ctx) {
    if (!ctx || !ctx->is_rendering) return false;

    // Сбрасываем незавершённый батч вершин на GPU
    vulkan_flush_batch(ctx);

    FrameData& frame = ctx->frames[ctx->current_frame];
    vkCmdEndRenderPass(frame.command_buffer);
    vkEndCommandBuffer(frame.command_buffer);

    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        // Ожидаем семафор готовности картинки из Swapchain
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &frame.image_available_sem,
        .pWaitDstStageMask    = wait_stages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &frame.command_buffer,
        // Сигнализируем по окончании рендеринга
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &frame.render_finished_sem
    };

    if (vkQueueSubmit(ctx->graphics_queue, 1, &submit_info, frame.in_flight_fence) != VK_SUCCESS) {
        ctx->is_rendering = false;
        return false;
    }

    VkPresentInfoKHR present_info = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &frame.render_finished_sem,
        .swapchainCount     = 1,
        .pSwapchains        = &ctx->swapchain,
        .pImageIndices      = &ctx->current_image_index,
        .pResults           = nullptr
    };

    VkResult res = vkQueuePresentKHR(ctx->present_queue, &present_info);

    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || window_was_resized(ctx->window)) {
        vulkan_context_recreate_swapchain(ctx);
        window_reset_resized_flag(ctx->window);
    }

    ctx->current_frame = (ctx->current_frame + 1) % VULKAN_MAX_FRAMES_IN_FLIGHT;
    ctx->is_rendering  = false;
    return true;
}

// ============================================================================
// Процедурная отрисовка: Страницы PDF, Подсветка текста, Рамки
// ============================================================================

void vulkan_draw_textured_rect(VulkanContext* ctx, const VulkanTexture* texture,
                               float x, float y, float width, float height,
                               float u0, float v0, float u1, float v1,
                               ColorRGBA tint) {
    if (!ctx || !ctx->is_rendering || width <= 0.0f || height <= 0.0f) return;

    // Если текстура не передана, используем внутреннюю 1x1 белую текстуру (для сплошного цвета)
    VkDescriptorSet target_ds = (texture && texture->desc_set) ? texture->desc_set : ctx->white_texture.desc_set;

    // Если текстура изменилась по сравнению с предыдущим вызовом — сбрасываем батч
    if (ctx->active_desc_set != target_ds) {
        vulkan_flush_batch(ctx);
        ctx->active_desc_set = target_ds;
    }

    uint32_t total_quads_drawn = (ctx->batch_draw_offset_indices / 6) + ctx->batch_quad_count;
    if (total_quads_drawn + 1 >= VULKAN_MAX_2D_QUADS) {
        vulkan_flush_batch(ctx);
        return; // Защита от переполнения динамического буфера
    }

    FrameData& frame = ctx->frames[ctx->current_frame];
    uint32_t vert_start = total_quads_drawn * 4;
    uint32_t idx_start  = total_quads_drawn * 6;

    Vertex2D* v = &frame.mapped_vertices[vert_start];
    // 0: Верхний левый угол
    v[0] = { .pos = { x, y },                  .uv = { u0, v0 }, .color = { tint.r, tint.g, tint.b, tint.a } };
    // 1: Верхний правый угол
    v[1] = { .pos = { x + width, y },          .uv = { u1, v0 }, .color = { tint.r, tint.g, tint.b, tint.a } };
    // 2: Нижний правый угол
    v[2] = { .pos = { x + width, y + height }, .uv = { u1, v1 }, .color = { tint.r, tint.g, tint.b, tint.a } };
    // 3: Нижний левый угол
    v[3] = { .pos = { x, y + height },          .uv = { u0, v1 }, .color = { tint.r, tint.g, tint.b, tint.a } };

    uint32_t* idx = &frame.mapped_indices[idx_start];
    // Два треугольника на каждый четырёхугольник (quad)
    idx[0] = vert_start + 0;
    idx[1] = vert_start + 1;
    idx[2] = vert_start + 2;
    idx[3] = vert_start + 2;
    idx[4] = vert_start + 3;
    idx[5] = vert_start + 0;

    ctx->batch_quad_count++;
}

void vulkan_draw_rect(VulkanContext* ctx, float x, float y, float width, float height, ColorRGBA color) {
    vulkan_draw_textured_rect(ctx, &ctx->white_texture, x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, color);
}

void vulkan_draw_highlight(VulkanContext* ctx, float x, float y, float width, float height, ColorRGBA color) {
    // Подсветка текста озвучки — это полупрозрачный прямоугольник, мягко смешиваемый поверх текста страницы PDF
    vulkan_draw_rect(ctx, x, y, width, height, color);
}

void vulkan_draw_rect_outline(VulkanContext* ctx, float x, float y, float width, float height,
                              float thickness, ColorRGBA color) {
    if (thickness <= 0.0f) return;
    // Верхняя грань
    vulkan_draw_rect(ctx, x, y, width, thickness, color);
    // Нижняя грань
    vulkan_draw_rect(ctx, x, y + height - thickness, width, thickness, color);
    // Левая грань
    vulkan_draw_rect(ctx, x, y + thickness, thickness, height - 2.0f * thickness, color);
    // Правая грань
    vulkan_draw_rect(ctx, x + width - thickness, y + thickness, thickness, height - 2.0f * thickness, color);
}
