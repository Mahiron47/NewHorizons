#pragma once

#include "windows.hpp"

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>
#include <cstdint>
#include <cstddef>

// Максимальное количество 2D прямоугольников в одном кадре отрисовки (для динамического батчинга)
constexpr uint32_t VULKAN_MAX_2D_QUADS        = 16384;
constexpr uint32_t VULKAN_MAX_2D_VERTS        = VULKAN_MAX_2D_QUADS * 4;
constexpr uint32_t VULKAN_MAX_2D_INDICES      = VULKAN_MAX_2D_QUADS * 6;
constexpr uint32_t VULKAN_MAX_FRAMES_IN_FLIGHT = 2;

// Вершина для 2D геометрии (страниц PDF, подсветок текста, рамок и элементов UI)
struct Vertex2D {
    float pos[2];    // x, y в экранных пикселях (0,0 — верхний левый угол)
    float uv[2];     // u, v координаты текстуры
    float color[4];  // r, g, b, a (0.0f - 1.0f)
};

// Текстура Vulkan (для растрированной страницы PDF или изображений)
struct VulkanTexture {
    VkImage image               = VK_NULL_HANDLE;
    VkDeviceMemory memory       = VK_NULL_HANDLE;
    VkImageView view            = VK_NULL_HANDLE;
    VkDescriptorSet desc_set    = VK_NULL_HANDLE;
    uint32_t width              = 0;
    uint32_t height             = 0;
};

// Цвет RGBA (0.0f - 1.0f)
struct ColorRGBA {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

// Конфигурация контекста Vulkan
struct VulkanConfig {
    bool enable_validation = true; // Валидационные слои в режиме отладки
    bool vsync             = true; // Вертикальная синхронизация (FIFO)
};

// Данные синхронизации и буферов для одного кадра в полёте
struct FrameData {
    VkCommandPool command_pool       = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer   = VK_NULL_HANDLE;
    VkSemaphore image_available_sem  = VK_NULL_HANDLE;
    VkSemaphore render_finished_sem  = VK_NULL_HANDLE;
    VkFence in_flight_fence          = VK_NULL_HANDLE;

    // Динамические буферы вершин и индексов
    VkBuffer vertex_buffer           = VK_NULL_HANDLE;
    VkDeviceMemory vertex_memory     = VK_NULL_HANDLE;
    Vertex2D* mapped_vertices        = nullptr;

    VkBuffer index_buffer            = VK_NULL_HANDLE;
    VkDeviceMemory index_memory      = VK_NULL_HANDLE;
    uint32_t* mapped_indices         = nullptr;
};

// Основной процедурный контекст Vulkan
struct VulkanContext {
    Window* window                        = nullptr;

    VkInstance instance                   = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_msgr   = VK_NULL_HANDLE;
    VkSurfaceKHR surface                  = VK_NULL_HANDLE;

    VkPhysicalDevice physical_device      = VK_NULL_HANDLE;
    VkDevice device                       = VK_NULL_HANDLE;

    uint32_t graphics_queue_family        = 0;
    uint32_t present_queue_family         = 0;
    VkQueue graphics_queue                = VK_NULL_HANDLE;
    VkQueue present_queue                 = VK_NULL_HANDLE;

    // Swapchain
    VkSwapchainKHR swapchain              = VK_NULL_HANDLE;
    VkFormat swapchain_format             = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchain_extent           = {0, 0};
    uint32_t swapchain_image_count        = 0;
    VkImage* swapchain_images             = nullptr;
    VkImageView* swapchain_image_views    = nullptr;
    VkFramebuffer* swapchain_framebuffers = nullptr;

    // Рендерпас и пайплайн 2D отрисовки
    VkRenderPass render_pass              = VK_NULL_HANDLE;
    VkDescriptorSetLayout desc_layout     = VK_NULL_HANDLE;
    VkDescriptorPool desc_pool            = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout      = VK_NULL_HANDLE;
    VkPipeline pipeline_2d                = VK_NULL_HANDLE;
    VkSampler default_sampler             = VK_NULL_HANDLE;

    // Белая текстура 1x1 для отрисовки сплошных цветов (подсветка текста, фон, рамки)
    VulkanTexture white_texture;

    // Кадры в полёте
    FrameData frames[VULKAN_MAX_FRAMES_IN_FLIGHT];
    uint32_t current_frame                = 0;
    uint32_t current_image_index          = 0;
    bool is_rendering                     = false;

    // Текущий батч отрисовки
    uint32_t batch_quad_count             = 0;
    uint32_t batch_draw_offset_indices    = 0;
    VkDescriptorSet active_desc_set       = VK_NULL_HANDLE;
};

// ============================================================================
// Процедурный API Vulkan Context
// ============================================================================

// --- Жизненный цикл контекста ---
bool vulkan_context_init(VulkanContext* ctx, Window* window, const VulkanConfig* config);
void vulkan_context_cleanup(VulkanContext* ctx);
bool vulkan_context_recreate_swapchain(VulkanContext* ctx);

// --- Работа с текстурами (страницы PDF, изображения) ---
// Создание новой текстуры из буфера пикселей RGBA32
bool vulkan_create_texture(VulkanContext* ctx, uint32_t width, uint32_t height, const void* rgba_pixels, VulkanTexture* out_texture);
// Быстрое обновление содержимого текстуры (при смене страницы или масштабировании PDF)
bool vulkan_update_texture(VulkanContext* ctx, VulkanTexture* texture, uint32_t width, uint32_t height, const void* rgba_pixels);
// Освобождение ресурсов текстуры
void vulkan_destroy_texture(VulkanContext* ctx, VulkanTexture* texture);

// --- Цикл рендеринга кадра ---
// Начало кадра (с очисткой заданным цветом фона)
bool vulkan_begin_frame(VulkanContext* ctx, float clear_r, float clear_g, float clear_b, float clear_a);
// Завершение кадра, отправка буферов и презентация на экран
bool vulkan_end_frame(VulkanContext* ctx);

// --- Графические примитивы для чтения PDF и подсветки текста озвучки ---

// 1. Отрисовка страницы PDF (текстурированный прямоугольник с заданными координатами и размером)
void vulkan_draw_textured_rect(VulkanContext* ctx, const VulkanTexture* texture,
                               float x, float y, float width, float height,
                               float u0 = 0.0f, float v0 = 0.0f, float u1 = 1.0f, float v1 = 1.0f,
                               ColorRGBA tint = {1.0f, 1.0f, 1.0f, 1.0f});

// 2. Отрисовка сплошного цветного прямоугольника (фон страницы, панели интерфейса)
void vulkan_draw_rect(VulkanContext* ctx, float x, float y, float width, float height, ColorRGBA color);

// 3. Подсветка текста, читаемого голосовой моделью (полупрозрачный маркер слова или предложения)
// Например: ColorRGBA{1.0f, 0.92f, 0.23f, 0.40f} — мягкий жёлтый цвет выделителя текста
void vulkan_draw_highlight(VulkanContext* ctx, float x, float y, float width, float height, ColorRGBA color);

// 4. Отрисовка рамки или подчеркивания (для курсора чтения, фокуса слова или границ документа)
void vulkan_draw_rect_outline(VulkanContext* ctx, float x, float y, float width, float height,
                              float thickness, ColorRGBA color);

// Сброс текущего накопленного батча на видеокарту
void vulkan_flush_batch(VulkanContext* ctx);

