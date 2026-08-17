#include "Hwindow.hpp"

LRESULT CALLBACK Hwindow::WindowProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
    Hwindow* pWindow = nullptr;

    // l_param contains a pointer to the CREATESTRUCT structure
    // which contains the lpCreateParams member that we passed to CreateWindowEx
    // This is how we retrieve the Hwindow instance associated with this window
    // When the window is created, we store the pointer to the Hwindow instance in the window's user data
    // When another message is received, we retrieve the pointer from the window's user data
    if (u_msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(l_param);
        pWindow = reinterpret_cast<Hwindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
    } else {
        pWindow = reinterpret_cast<Hwindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pWindow != nullptr) {
        switch (u_msg) {
            case WM_CLOSE:
                pWindow->_running = false;
                DestroyWindow(hwnd);
                return 0;

            case WM_DESTROY:
                pWindow->_running = false;
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                pWindow->_width = LOWORD(l_param);
                pWindow->_height = HIWORD(l_param);
                return 0;
        }
    }

    return DefWindowProc(hwnd, u_msg, w_param, l_param);
}

Hwindow* Hwindow::initialize(WTEXT title, uint32_t width, uint32_t height) {
    HINSTANCE h_instance = GetModuleHandle(nullptr);
    WTEXT class_name = TEXT("HorizonsWindowClass");
    Hwindow* window = new Hwindow(nullptr, h_instance, class_name, title, width, height);
    
    WNDCLASSEX windows_class = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .hInstance = h_instance,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszClassName = class_name
    };
    
    if (!RegisterClassEx(&windows_class)) throw std::runtime_error("Hwindow::initialize : Failed to register window class.");
    
    RECT rect = { 
        0,            0, 
        (LONG) width, (LONG) height 
    };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    
    HWND hwnd = CreateWindowEx(
        0,
        class_name,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        h_instance,
        window
    );
    
    if (hwnd == nullptr) throw std::runtime_error("Hwindow::initialize : Failed to create window.");
    
    return window;
}

void Hwindow::show() const {
    if (_hwnd == nullptr) throw std::runtime_error("Hwindow::show : Window handle is null.");
    
    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);
}

void Hwindow::hide() const {
    if (_hwnd == nullptr) throw std::runtime_error("Hwindow::hide : Window handle is null.");
    
    ShowWindow(_hwnd, SW_HIDE);
}

bool Hwindow::is_running() const {
    return _running;
}

void Hwindow::process_messages() {
    MSG msg = {};
    while (PeekMessage(&msg, _hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            _running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
