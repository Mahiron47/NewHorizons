#include "Hwindow.hpp"

Hstack<uint32_t, Hwindow> Hwindow::windows;

LRESULT CALLBACK Hwindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Hwindow* pWindow = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pWindow = reinterpret_cast<Hwindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
    } else {
        pWindow = reinterpret_cast<Hwindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pWindow) {
        switch (uMsg) {
            case WM_CLOSE:
                pWindow->running = false;
                DestroyWindow(hwnd);
                return 0;

            case WM_DESTROY:
                pWindow->running = false;
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                pWindow->width = LOWORD(lParam);
                pWindow->height = HIWORD(lParam);
                return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Hwindow& Hwindow::initialize(uint32_t width, uint32_t height) {
    Hwindow::windows.push(Hwindow{});
    Hwindow& instance = Hwindow::windows.last();

    WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszClassName = TEXT("HorizonsWindowClass")
    };

    if (!RegisterClassEx(&wc)) {
        Hwindow::windows.pop();
        throw std::runtime_error("Hwindow::initialize : Failed to register window class.");
    }

    RECT rect = { 0, 0, (LONG) width, (LONG) height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(
        0,
        TEXT("HorizonsWindowClass"),
        TEXT("Horizons Engine"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        &instance
    );

    if (!hwnd) {
        Hwindow::windows.pop();
        throw std::runtime_error("Hwindow::initialize : Failed to create window.");
    }

	instance = Hwindow(hwnd, GetModuleHandle(nullptr), TEXT("HorizonsWindowClass"), TEXT("Horizons Engine"), width, height);
    return instance;
}

void Hwindow::show() {
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}

void Hwindow::hide() {
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
}

bool Hwindow::isRunning() const {
    return running;
}

void Hwindow::processMessages() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
