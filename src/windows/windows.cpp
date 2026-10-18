#include "windows.hpp"
#include <string>

// Статическая процедура обработки сообщений Win32
static LRESULT CALLBACK window_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_NCCREATE: {
            CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lparam);
            window = reinterpret_cast<Window*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }

        case WM_CLOSE: {
            if (window) {
                window->should_close = true;
            }
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_SIZE: {
            if (window) {
                if (wparam == SIZE_MINIMIZED) {
                    window->minimized = true;
                } else {
                    window->minimized = false;
                    window->width = LOWORD(lparam);
                    window->height = HIWORD(lparam);
                    window->resized = true;
                }
            }
            return 0;
        }

        case WM_PAINT: {
            // В Vulkan отрисовка не использует GDI, но необходимо подтвердить регион,
            // чтобы Windows не отправляла бесконечный поток сообщений WM_PAINT
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND: {
            // Сообщаем системе, что очистка фона выполнена, для устранения мерцания
            return 1;
        }

        default:
            return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}

static void enable_dpi_awareness() {
    // Включаем Per-Monitor DPI Awareness для четкого рендеринга и точных пикселей
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        typedef BOOL(WINAPI* SetProcessDpiAwarenessContextProc)(DPI_AWARENESS_CONTEXT);
        auto set_dpi_ctx = reinterpret_cast<SetProcessDpiAwarenessContextProc>(
            GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        if (set_dpi_ctx) {
            set_dpi_ctx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            return;
        }
    }
    SetProcessDPIAware();
}

bool window_init(Window* window, const WindowConfig* config) {
    if (!window) return false;

    WindowConfig default_config;
    if (!config) {
        config = &default_config;
    }

    enable_dpi_awareness();

    HINSTANCE hinstance = GetModuleHandleW(nullptr);
    const wchar_t* class_name = L"Horizon_Vulkan_Window_Class";

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = window_wnd_proc;
    wc.hInstance     = hinstance;
    wc.hCursor       = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.lpszClassName = class_name;

    RegisterClassExW(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW;
    if (!config->resizable) {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }

    RECT wr = { 0, 0, static_cast<LONG>(config->width), static_cast<LONG>(config->height) };
    AdjustWindowRect(&wr, style, FALSE);

    int window_width  = wr.right - wr.left;
    int window_height = wr.bottom - wr.top;

    int screen_width  = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int pos_x = (screen_width - window_width) / 2;
    int pos_y = (screen_height - window_height) / 2;
    if (pos_x < 0) pos_x = CW_USEDEFAULT;
    if (pos_y < 0) pos_y = CW_USEDEFAULT;

    // Конвертируем заголовок из UTF-8 в UTF-16
    int title_len = MultiByteToWideChar(CP_UTF8, 0, config->title, -1, nullptr, 0);
    std::wstring wtitle(title_len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, config->title, -1, wtitle.data(), title_len);

    HWND hwnd = CreateWindowExW(
        0,
        class_name,
        wtitle.c_str(),
        style,
        pos_x, pos_y,
        window_width, window_height,
        nullptr,
        nullptr,
        hinstance,
        window
    );

    if (!hwnd) {
        return false;
    }

    window->hwnd         = hwnd;
    window->hinstance    = hinstance;
    window->width        = config->width;
    window->height       = config->height;
    window->should_close = false;
    window->resized      = false;
    window->minimized    = false;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return true;
}

void window_destroy(Window* window) {
    if (!window) return;

    if (window->hwnd) {
        DestroyWindow(window->hwnd);
        window->hwnd = nullptr;
    }

    UnregisterClassW(L"Horizon_Vulkan_Window_Class", window->hinstance);
    window->hinstance    = nullptr;
    window->should_close = true;
}

void window_poll_events(Window* window) {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            if (window) {
                window->should_close = true;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

bool window_should_close(const Window* window) {
    return window ? window->should_close : true;
}

bool window_was_resized(const Window* window) {
    return window ? window->resized : false;
}

void window_reset_resized_flag(Window* window) {
    if (window) {
        window->resized = false;
    }
}

bool window_is_minimized(const Window* window) {
    return window ? window->minimized : false;
}

void window_get_framebuffer_size(const Window* window, uint32_t* out_width, uint32_t* out_height) {
    if (!window || !window->hwnd) {
        if (out_width) *out_width = 0;
        if (out_height) *out_height = 0;
        return;
    }

    RECT rect;
    GetClientRect(window->hwnd, &rect);
    if (out_width) *out_width = static_cast<uint32_t>(rect.right - rect.left);
    if (out_height) *out_height = static_cast<uint32_t>(rect.bottom - rect.top);
}

HWND window_get_hwnd(const Window* window) {
    return window ? window->hwnd : nullptr;
}

HINSTANCE window_get_hinstance(const Window* window) {
    return window ? window->hinstance : nullptr;
}

const char* const* window_get_required_instance_extensions(uint32_t* out_count) {
    static const char* extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };
    if (out_count) {
        *out_count = static_cast<uint32_t>(sizeof(extensions) / sizeof(extensions[0]));
    }
    return extensions;
}

#if defined(HORIZON_VULKAN_SUPPORTED)
VkResult window_create_vulkan_surface(const Window* window,
                                     VkInstance instance,
                                     const VkAllocationCallbacks* allocator,
                                     VkSurfaceKHR* out_surface) {
    if (!window || !window->hwnd || !instance || !out_surface) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkWin32SurfaceCreateInfoKHR create_info{};
    create_info.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.pNext     = nullptr;
    create_info.flags     = 0;
    create_info.hinstance = window->hinstance;
    create_info.hwnd      = window->hwnd;

    return vkCreateWin32SurfaceKHR(instance, &create_info, allocator, out_surface);
}
#endif
