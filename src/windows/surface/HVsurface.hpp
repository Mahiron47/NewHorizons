#ifndef HVSURFACEHPP
#define HVSURFACEHPP

#include <vulkan/vulkan_win32.h>

class HVsurface {
	VkSurfaceKHR _surface;

	HVinstance*  _instance;

	HVsurface(VkSurfaceKHR surface, 
			  HVinstance*  instance) : _surface(surface),
									   _instance(instance) {}

public:
	static HVsurface* initialize(HVinstance* instance, Hwindow* window);

	constexpr operator VkSurfaceKHR() const { return _surface; }

   ~HVsurface() {
		if (_surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(*_instance, _surface, nullptr);
	}
	HVsurface(HVsurface&& other) noexcept : _instance(other._instance), 
											_surface(other._surface) {
		other._instance = VK_NULL_HANDLE;
		other._surface = VK_NULL_HANDLE;
	}
	HVsurface& operator=(HVsurface&& other) noexcept {
		if (this == &other) return *this;
        
        this->_instance = other._instance;
        this->_surface = other._surface;

        other._instance = VK_NULL_HANDLE;
        other._surface = VK_NULL_HANDLE;

        return *this;
	}
	HVsurface(const HVsurface&) = delete;
	HVsurface& operator=(const HVsurface&) = delete;
	
};

inline HVsurface* HVsurface::initialize(HVinstance* instance, Hwindow* window) {
    VkSurfaceKHR surface;

    VkResult result = VK_ERROR_UNKNOWN;

    VkWin32SurfaceCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = window->get_h_instance(),
        .hwnd = window->get_handle()
    };

    result = vkCreateWin32SurfaceKHR(*instance, &createInfo, nullptr, &surface);
    
    if (result != VK_SUCCESS) throw std::runtime_error("HVsurface:initialize : Failed to create window surface.");

    return new HVsurface(surface, instance);
}

#endif // HVSURFACEHPP