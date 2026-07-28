#ifndef HRENDERTHREAD_HPP
#define HRENDERTHREAD_HPP

#include "Horizon.hpp"
#include "vulkan/instance/HVinstance.hpp"
#include "vulkan/surface/HVsurface.hpp"
#include "vulkan/phys_device/HVphysicalDevice.hpp"
#include "vulkan/device/HVdevice.hpp"

class HrenderThread {
	HVinstance* instance;
	Hwindow* window;
	HVsurface* surface;

	HrenderThread();

protected:

#ifdef DEBUG_TESTS

// TODO: tests cases of 3d graphics, 
// 						   compute pipelines, 
// 					       memory management, 
// 					   	   synchronization primitives, etc.

#endif
public:
	static HrenderThread initialize() {
		return HrenderThread();
	}

	void execute() {
		window->show();
		while (window->isRunning()) {
			window->processMessages();
			// rendering part
		}
	}

};

/*
VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
*/

#endif // HMAINTHREAD_HPP