#ifndef HV_SURFACE_HPP
#define HV_SURFACE_HPP
	
#include "vulkan/instance/HVinstance.hpp"
#include "window/Hwindow.hpp"
#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>

class HVsurface {
	static Hstack<uint32_t, HVsurface> surfaces;

// vulkan fields
 	VkInstance instance;
	VkSurfaceKHR surface;

	HVsurface(VkInstance instance, 
			  VkSurfaceKHR surface) : instance(instance), 
			  						  surface(surface) {}
public:
	static HVsurface& initialize(HVinstance& instance, Hwindow& window);

	static VkSurfaceKHR operator[](uint32_t index) { return surfaces[index]; }

	operator VkSurfaceKHR() const { return surface; }

	HVsurface(HVsurface&& other) noexcept : instance(other.instance), surface(other.surface) {
		other.instance = VK_NULL_HANDLE;
		other.surface = VK_NULL_HANDLE;
	}

	HVsurface& operator=(HVsurface&& other) noexcept {
		if (this != &other) { 
			if (this->surface != VK_NULL_HANDLE) {
				vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
			}
			
			this->instance = other.instance;
			this->surface = other.surface;

			other.instance = VK_NULL_HANDLE;
			other.surface = VK_NULL_HANDLE;
		}

		return *this;
	}

	HVsurface() : instance(VK_NULL_HANDLE),  
				  surface(VK_NULL_HANDLE) {}

   ~HVsurface() {
		if (this->surface != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
		}
	}

	HVsurface(const HVsurface&) = delete;
	HVsurface& operator=(const HVsurface&) = delete;
};

#endif // HV_SURFACE_HPP