#include "HVinstanceExtensions.hpp"

HVinstanceExtensions::HVinstanceExtensions(std::initializer_list<const char*> extensions) {
	
    auto comparator = [extensions](const char* extension) -> bool {
		std::for_each(extensions.begin(), extensions.end(), [extension](const char* ext) {
			if (strcmp(extension, ext) == 0) {
				return true;
			}
		});
	};

	this->extensions_list = {
		.vk_khr_surface 		= comparator(this->extensions_list.vk_khr_surface_name),
		.vk_khr_win32_surface 	= comparator(this->extensions_list.vk_khr_win32_surface_name),
		.vk_khr_xcb_surface 	= comparator(this->extensions_list.vk_khr_xcb_surface_name),
		.vk_khr_wayland_surface = comparator(this->extensions_list.vk_khr_wayland_surface_name),
		.vk_ext_debug_utils 	= comparator(this->extensions_list.vk_ext_debug_utils_name)
	};

    this->init_extensions();
}

void HVinstanceExtensions::init_extensions() {
    /* EXTENSIONS AVAILABILITY CHECK */ {
        VkResult result;

        uint32_t available_extensions_count;
        result = vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstanceExtensions::init_extensions : Failed to enumerate instance extension properties!");
        }

        std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, available_extensions.data());

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstanceExtensions::init_extensions : Failed to enumerate instance extension properties!");
        }

		auto extensions = available_extensions; 

		auto comparator = [extensions](const char* extension) -> bool {
			std::for_each(extensions.begin(), extensions.end(), [extension](const char* ext) {
				if (strcmp(extension, ext) == 0) {
					return true;
				}
			});
		};

		if (this->extensions_list.vk_khr_surface 			!= comparator(this->extensions_list.vk_khr_surface_name)) {
			std::cout << "Extension VK_KHR_surface is not supported." 			<< std::endl;
		} if (this->extensions_list.vk_khr_win32_surface 	!= comparator(this->extensions_list.vk_khr_win32_surface_name)) {
			std::cout << "Extension VK_KHR_win32_surface is not supported." 	<< std::endl;
		} if (this->extensions_list.vk_khr_xcb_surface 		!= comparator(this->extensions_list.vk_khr_xcb_surface_name)) {
			std::cout << "Extension VK_KHR_xcb_surface is not supported." 		<< std::endl;
		} if (this->extensions_list.vk_khr_wayland_surface 	!= comparator(this->extensions_list.vk_khr_wayland_surface_name)) {
			std::cout << "Extension VK_KHR_wayland_surface is not supported." 	<< std::endl;
		} if (this->extensions_list.vk_ext_debug_utils 		!= comparator(this->extensions_list.vk_ext_debug_utils_name)) {
			std::cout << "Extension VK_EXT_debug_utils is not supported." 		<< std::endl;
		}
         
    } /* EXTENSIONS AVAILABILITY CHECK */
}