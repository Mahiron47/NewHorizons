#include "HVdeviceExtensions.hpp"

HVdeviceExtensions::HVdeviceExtensions(std::initializer_list<const char*> extensions) {

    //auto comparator = [extensions](const char* extension) -> bool {
	//	std::for_each(extensions.begin(), extensions.end(), [extension](const char* ext) {
	//		if (strcmp(extension, ext) == 0) {
	//			return true;
	//		}
	//	});
	//};
//
	//this->extensions_list = {
	//	.vk_ext_descriptor_indexing 			= comparator(this->extensions_list.vk_ext_descriptor_indexing_name),
	//	.vk_khr_buffer_device_address 			= comparator(this->extensions_list.vk_khr_buffer_device_address_name),
	//	.vk_ext_scalar_block_layout 			= comparator(this->extensions_list.vk_ext_scalar_block_layout_name),
	//	.vk_khr_uniform_buffer_standard_layout 	= comparator(this->extensions_list.vk_khr_uniform_buffer_standard_layout_name),
	//	.vk_ext_inline_uniform_block 			= comparator(this->extensions_list.vk_ext_inline_uniform_block_name),
	//	.vk_khr_push_descriptor 				= comparator(this->extensions_list.vk_khr_push_descriptor_name),
	//	.vk_khr_dynamic_rendering 				= comparator(this->extensions_list.vk_khr_dynamic_rendering_name),
	//	.vk_khr_synchronization2 				= comparator(this->extensions_list.vk_khr_synchronization2_name),
	//	.vk_khr_timeline_semaphore 				= comparator(this->extensions_list.vk_khr_timeline_semaphore_name),
	//	.vk_khr_copy_commands2 					= comparator(this->extensions_list.vk_khr_copy_commands2_name),
	//	.vk_ext_extended_dynamic_state 			= comparator(this->extensions_list.vk_ext_extended_dynamic_state_name),
	//	.vk_ext_extended_dynamic_state2 		= comparator(this->extensions_list.vk_ext_extended_dynamic_state2_name)
	//};

    this->init_extensions();
}

std::vector<const char*> HVdeviceExtensions::getExtensionsAsVector() const {
    return std::vector<const char*>{
		(this->extensions_list.vk_ext_descriptor_indexing ? this->extensions_list.vk_ext_descriptor_indexing_name : nullptr),
		(this->extensions_list.vk_khr_buffer_device_address ? this->extensions_list.vk_khr_buffer_device_address_name : nullptr),
		(this->extensions_list.vk_ext_scalar_block_layout ? this->extensions_list.vk_ext_scalar_block_layout_name : nullptr),
		(this->extensions_list.vk_khr_uniform_buffer_standard_layout ? this->extensions_list.vk_khr_uniform_buffer_standard_layout_name : nullptr),
		(this->extensions_list.vk_ext_inline_uniform_block ? this->extensions_list.vk_ext_inline_uniform_block_name : nullptr),
		(this->extensions_list.vk_khr_push_descriptor ? this->extensions_list.vk_khr_push_descriptor_name : nullptr),
		(this->extensions_list.vk_khr_dynamic_rendering ? this->extensions_list.vk_khr_dynamic_rendering_name : nullptr),
		(this->extensions_list.vk_khr_synchronization2 ? this->extensions_list.vk_khr_synchronization2_name : nullptr),
		(this->extensions_list.vk_khr_timeline_semaphore ? this->extensions_list.vk_khr_timeline_semaphore_name : nullptr),
		(this->extensions_list.vk_khr_copy_commands2 ? this->extensions_list.vk_khr_copy_commands2_name : nullptr),
		(this->extensions_list.vk_ext_extended_dynamic_state ? this->extensions_list.vk_ext_extended_dynamic_state_name : nullptr),
		(this->extensions_list.vk_ext_extended_dynamic_state2 ? this->extensions_list.vk_ext_extended_dynamic_state2_name : nullptr)
	};
}

void HVdeviceExtensions::init_extensions() {
    /* EXTENSIONS AVAILABILITY CHECK */ {
        VkResult result;

        uint32_t available_extensions_count;
        result = vkEnumerateDeviceExtensionProperties(nullptr, nullptr, &available_extensions_count, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVdeviceExtensions::init_extensions : Failed to enumerate device extension properties!");
        }

        std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
        result = vkEnumerateDeviceExtensionProperties(nullptr, nullptr, &available_extensions_count, available_extensions.data());

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVdeviceExtensions::init_extensions : Failed to enumerate device extension properties!");
        }

		auto extensions = available_extensions; 

		//auto comparator = [extensions](const char* extension) -> bool {
		//	std::for_each(extensions.begin(), extensions.end(), [extension](const VkExtensionProperties& ext) {
		//		if (strcmp(extension, ext.extensionName) == 0) {
		//			return true;
		//		}
		//		return false;
		//	});
		//};
//
		//if (this->extensions_list.vk_ext_descriptor_indexing 				!= comparator(this->extensions_list.vk_ext_descriptor_indexing_name)) {
		//	std::cout << "Extension VK_EXT_descriptor_indexing is not supported." 				<< std::endl;
		//} if (this->extensions_list.vk_khr_buffer_device_address 			!= comparator(this->extensions_list.vk_khr_buffer_device_address_name)) {
		//	std::cout << "Extension VK_KHR_buffer_device_address is not supported." 			<< std::endl;
		//} if (this->extensions_list.vk_ext_scalar_block_layout 				!= comparator(this->extensions_list.vk_ext_scalar_block_layout_name)) {
		//	std::cout << "Extension VK_EXT_scalar_block_layout is not supported." 				<< std::endl;
		//} if (this->extensions_list.vk_khr_uniform_buffer_standard_layout 	!= comparator(this->extensions_list.vk_khr_uniform_buffer_standard_layout_name)) {
		//	std::cout << "Extension VK_KHR_uniform_buffer_standard_layout is not supported." 	<< std::endl;
		//} if (this->extensions_list.vk_ext_inline_uniform_block 			!= comparator(this->extensions_list.vk_ext_inline_uniform_block_name)) {
		//	std::cout << "Extension VK_EXT_inline_uniform_block is not supported." 				<< std::endl;  
		//} if (this->extensions_list.vk_khr_push_descriptor 					!= comparator(this->extensions_list.vk_khr_push_descriptor_name)) {
		//	std::cout << "Extension VK_KHR_push_descriptor is not supported." 					<< std::endl;
		//} if (this->extensions_list.vk_khr_dynamic_rendering 				!= comparator(this->extensions_list.vk_khr_dynamic_rendering_name)) {
		//	std::cout << "Extension VK_KHR_dynamic_rendering is not supported." 				<< std::endl;
		//} if (this->extensions_list.vk_khr_synchronization2 				!= comparator(this->extensions_list.vk_khr_synchronization2_name)) {
		//	std::cout << "Extension VK_KHR_synchronization2 is not supported." 					<< std::endl;
		//} if (this->extensions_list.vk_khr_timeline_semaphore 				!= comparator(this->extensions_list.vk_khr_timeline_semaphore_name)) {
		//	std::cout << "Extension VK_KHR_timeline_semaphore is not supported." 				<< std::endl;
		//} if (this->extensions_list.vk_khr_copy_commands2 					!= comparator(this->extensions_list.vk_khr_copy_commands2_name)) {
		//	std::cout << "Extension VK_KHR_copy_commands2 is not supported." 					<< std::endl;
		//} if (this->extensions_list.vk_ext_extended_dynamic_state 			!= comparator(this->extensions_list.vk_ext_extended_dynamic_state_name)) {
		//	std::cout << "Extension VK_EXT_extended_dynamic_state is not supported." 			<< std::endl;
		//} if (this->extensions_list.vk_ext_extended_dynamic_state2 			!= comparator(this->extensions_list.vk_ext_extended_dynamic_state2_name)) {
		//	std::cout << "Extension VK_EXT_extended_dynamic_state2 is not supported." 			<< std::endl;
		//}
         
    } /* EXTENSIONS AVAILABILITY CHECK */
}