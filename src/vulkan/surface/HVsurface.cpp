#include "HVsurface.hpp"

Hstack<uint32_t, HVsurface> HVsurface::surfaces;

HVsurface& HVsurface::initialize(HVinstance& instance, Hwindow& window) {
    VkSurfaceKHR surface;

    /* SURFACE CREATION WITH SUCCESS CHECK */ {
        VkWin32SurfaceCreateInfoKHR createInfo {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandle(nullptr),
            .hwnd = window.getHandle()
        };

        VkResult result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVsurface:initialize : Failed to create window surface!");
        }
    } /* SURFACE CREATION WITH SUCCESS CHECK */
    
    HVsurface::surfaces.push(HVsurface(instance, surface));
    return HVsurface::surfaces.last();
}