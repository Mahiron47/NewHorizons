#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <cstdint>

#if __has_include(<vulkan/vulkan.h>)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#define HORIZON_VULKAN_SUPPORTED 1
#endif

// Конфигурация создания окна
struct WindowConfig {
    const char* title = "New Horizons (Vulkan)";
    uint32_t width    = 1280;
    uint32_t height   = 720;
    bool resizable    = true;
};

// Состояние и дескрипторы окна
struct Window {
    HWND hwnd             = nullptr;
    HINSTANCE hinstance   = nullptr;
    uint32_t width        = 0;
    uint32_t height       = 0;
    bool should_close     = false;
    bool resized          = false;
    bool minimized        = false;
    void* user_data       = nullptr;
};

// --- Процедурный API управления окном ---

// Инициализация и создание окна
bool window_init(Window* window, const WindowConfig* config);

// Уничтожение окна и освобождение системных ресурсов
void window_destroy(Window* window);

// Обработка системных событий и очереди сообщений
void window_poll_events(Window* window);

// Проверка необходимости закрытия окна
bool window_should_close(const Window* window);

// Проверка и сброс флага изменения размера (для пересоздания Vulkan Swapchain)
bool window_was_resized(const Window* window);
void window_reset_resized_flag(Window* window);

// Проверка минимизации окна
bool window_is_minimized(const Window* window);

// Получение фактического размера клиентской области в пикселях (FrameBuffer)
void window_get_framebuffer_size(const Window* window, uint32_t* out_width, uint32_t* out_height);

// Получение нативных дескрипторов Win32
HWND window_get_hwnd(const Window* window);
HINSTANCE window_get_hinstance(const Window* window);

// Необходимые расширения экземпляра Vulkan (VK_KHR_surface, VK_KHR_win32_surface)
const char* const* window_get_required_instance_extensions(uint32_t* out_count);

#if defined(HORIZON_VULKAN_SUPPORTED)
// Создание поверхности Vulkan (VkSurfaceKHR) через расширение VK_KHR_win32_surface
VkResult window_create_vulkan_surface(const Window* window,
                                     VkInstance instance,
                                     const VkAllocationCallbacks* allocator,
                                     VkSurfaceKHR* out_surface);
#endif

