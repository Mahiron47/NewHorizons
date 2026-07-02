#ifndef HV_INSTANCE_EXTENSIONS_HPP
#define HV_INSTANCE_EXTENSIONS_HPP

#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <vulkan/vulkan.hpp>
#include <initializer_list>
#include <vector>

struct sHVinstanceExtensions {
	// 1 group
	bool vk_khr_surface;
	const char* vk_khr_surface_name 		= "VK_KHR_surface";
	bool vk_khr_win32_surface;
	const char* vk_khr_win32_surface_name 	= "VK_KHR_win32_surface";
	bool vk_khr_xcb_surface;
	const char* vk_khr_xcb_surface_name 	= "VK_KHR_xcb_surface";
	bool vk_khr_wayland_surface;
	const char* vk_khr_wayland_surface_name = "VK_KHR_wayland_surface";
	bool vk_ext_debug_utils;
	const char* vk_ext_debug_utils_name 	= "VK_EXT_debug_utils";

};

class HVinstanceExtensions {
	sHVinstanceExtensions extensions_list;


	void init_extensions();
	
public:
	HVinstanceExtensions(std::initializer_list<const char*> extensions);

	HVinstanceExtensions(const HVinstanceExtensions&);



};

#endif /* HV_INSTANCE_EXTENSIONS_HPP */