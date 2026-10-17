#ifndef WINDOWHPP
#define WINDOWHPP

struct HwindowExtent {
	uint32_t width;
	uint32_t height;

    constexpr operator VkExtent2D() { return VkExtent2D { .width = width, 
                                                          .height = height }; }
};

class Hwindow {
	HWND 	  	  _hwnd;
	HINSTANCE 	  _h_instance;
	LPCTSTR   	  _class_name;
	LPCTSTR   	  _title;
	HwindowExtent _extent;
	bool 	      _running;

	static LRESULT CALLBACK WindowProc(HWND hwnd, 
									   UINT u_msg, 
									   WPARAM w_param, 
									   LPARAM l_param);

	constexpr Hwindow(HWND hwnd, 
					  HINSTANCE h_instance, 
					  LPCTSTR class_name, 
					  LPCTSTR title, 
					  HwindowExtent extent) : _hwnd(hwnd), 
										      _h_instance(h_instance), 
										      _class_name(class_name), 
										      _title(title), 
										      _extent(extent), 
										      _running(true) {}

	constexpr void set_handle(HWND hwnd) noexcept { _hwnd = hwnd; }

public:
	static Hwindow* initialize(LPCTSTR title, LPCTSTR class_name, HwindowExtent extent);
	
	[[nodiscard]] constexpr bool is_running() const { return _running; }
	void process_messages();
	void show() const;
	void hide() const;

	[[nodiscard]] constexpr HWND get_handle() const noexcept { return _hwnd; }
    [[nodiscard]] constexpr HINSTANCE get_h_instance() const noexcept { return _h_instance; }
	[[nodiscard]] constexpr HwindowExtent get_extent() const noexcept { return _extent; }

	constexpr Hwindow() noexcept : _hwnd(nullptr), 
			    				   _h_instance(nullptr), 
			    				   _class_name(nullptr), 
			    				   _title(nullptr), 
			    				   _extent(HwindowExtent{}),
			    				   _running(false) {}

   ~Hwindow() {
		if (this->_hwnd != nullptr) DestroyWindow(this->_hwnd);
        
        if (this->_class_name != nullptr) UnregisterClass(this->_class_name, this->_h_instance);
	}
	constexpr Hwindow(Hwindow&& other) noexcept : _hwnd(other._hwnd),
					 							  _h_instance(other._h_instance),
					 							  _class_name(other._class_name),
					 							  _title(other._title),
					 							  _extent(other._extent),
					 							  _running(other._running) {
		other._hwnd = nullptr;
		other._h_instance = nullptr;
		other._class_name = nullptr;
		other._title = nullptr;
		other._extent = HwindowExtent{};
		other._running = false;
	}
	constexpr Hwindow& operator=(Hwindow&& other) noexcept {
		if (this != &other) return *this;
        
        this->_hwnd = other._hwnd;
        this->_h_instance = other._h_instance;
        this->_class_name = other._class_name;
        this->_title = other._title;
		this->_extent = other._extent;
        this->_running = other._running;
		
        other._hwnd = nullptr;
        other._h_instance = nullptr;
        other._class_name = nullptr;
        other._title = nullptr;
        other._extent = HwindowExtent{};
        other._running = false;
		
        return *this;
	}
    Hwindow(const Hwindow&) = delete;
	Hwindow& operator=(const Hwindow&) = delete;
};

inline LRESULT CALLBACK Hwindow::WindowProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
    Hwindow* p_window = nullptr;

    // l_param contains a pointer to the CREATESTRUCT structure
    // which contains the lpCreateParams member that we passed to CreateWindowEx
    // This is how we retrieve the Hwindow instance associated with this window
    // When the window is created, we store the pointer to the Hwindow instance in the window's user data
    // When another message is received, we retrieve the pointer from the window's user data
    if (u_msg == WM_NCCREATE) {
        CREATESTRUCT* p_create = reinterpret_cast<CREATESTRUCT*>(l_param); // NOLINT(performance-no-int-to-ptr)
        p_window = reinterpret_cast<Hwindow*>(p_create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_window));
    } else {
        p_window = reinterpret_cast<Hwindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)); // NOLINT(performance-no-int-to-ptr)
    }

    if (p_window != nullptr) {
        switch (u_msg) {
        case WM_CLOSE:
            p_window->_running = false;
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            p_window->_running = false;
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            p_window->_extent = HwindowExtent { LOWORD(l_param), 
                                                HIWORD(l_param) };
            return 0;
        }
    }

    return DefWindowProc(hwnd, u_msg, w_param, l_param);
}

inline Hwindow* Hwindow::initialize(LPCTSTR title, LPCTSTR class_name, HwindowExtent extent) {
    HINSTANCE h_instance = GetModuleHandle(nullptr);
    Hwindow* window = new Hwindow(nullptr, h_instance, class_name, title, extent);
    
    WNDCLASSEX windows_class = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .hInstance = h_instance,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), // NOLINT(performance-no-int-to-ptr)
        .lpszClassName = class_name
    };
    
    if (!RegisterClassEx(&windows_class)) throw std::runtime_error("Hwindow::initialize : Failed to register window class.");
    
    RECT rect = { 
        0,                   0, 
        (LONG) extent.width, (LONG) extent.height 
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
    
    window->set_handle(hwnd);

    return window;
}

inline void Hwindow::show() const {
    if (_hwnd == nullptr) throw std::runtime_error("Hwindow::show : Window handle is null.");
    
    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);
}

inline void Hwindow::hide() const {
    if (_hwnd == nullptr) throw std::runtime_error("Hwindow::hide : Window handle is null.");
    
    ShowWindow(_hwnd, SW_HIDE);
}

inline void Hwindow::process_messages() {
    MSG msg = {};
    while (PeekMessage(&msg, _hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            _running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#endif // WINDOWHPP