#ifndef HV_DEVICE_EXTENSIONS_HPP
#define HV_DEVICE_EXTENSIONS_HPP

#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <vulkan/vulkan.hpp>
#include <initializer_list>
#include <iostream>

struct sHVdeviceExtensions {
	// 1 group
	bool vk_ext_descriptor_indexing 						= false;
	const char* vk_ext_descriptor_indexing_name 			= "VK_EXT_descriptor_indexing";
	bool vk_khr_buffer_device_address 						= false;
	const char* vk_khr_buffer_device_address_name 			= "VK_KHR_buffer_device_address";
	bool vk_ext_scalar_block_layout 						= false;
	const char* vk_ext_scalar_block_layout_name 			= "VK_EXT_scalar_block_layout";
	bool vk_khr_uniform_buffer_standard_layout 				= false;
	const char* vk_khr_uniform_buffer_standard_layout_name 	= "VK_KHR_uniform_buffer_standard_layout";
	bool vk_ext_inline_uniform_block 						= false;
	const char* vk_ext_inline_uniform_block_name 			= "VK_EXT_inline_uniform_block";
	bool vk_khr_push_descriptor 							= false;
	const char* vk_khr_push_descriptor_name 				= "VK_KHR_push_descriptor";
	// 2 group
	bool vk_khr_dynamic_rendering 							= false;
	const char* vk_khr_dynamic_rendering_name 				= "VK_KHR_dynamic_rendering";
	bool vk_khr_synchronization2 							= false;
	const char* vk_khr_synchronization2_name 				= "VK_KHR_synchronization2";
	bool vk_khr_timeline_semaphore 							= false;
	const char* vk_khr_timeline_semaphore_name 				= "VK_KHR_timeline_semaphore";
	bool vk_khr_copy_commands2 								= false;
	const char* vk_khr_copy_commands2_name 					= "VK_KHR_copy_commands2";
	bool vk_ext_extended_dynamic_state  					= false;
	const char* vk_ext_extended_dynamic_state_name 			= "VK_EXT_extended_dynamic_state";
	bool vk_ext_extended_dynamic_state2 					= false;
	const char* vk_ext_extended_dynamic_state2_name 		= "VK_EXT_extended_dynamic_state2";

};

class HVdeviceExtensions {
	sHVdeviceExtensions extensions_list;


	void init_extensions();
	
public:
	HVdeviceExtensions(std::initializer_list<const char*> extensions);

	HVdeviceExtensions(const sHVdeviceExtensions& extensions) : extensions_list(extensions) {};

	std::vector<const char*> getExtensionsAsVector() const;

};

#endif /* HV_DEVICE_EXTENSIONS_HPP */