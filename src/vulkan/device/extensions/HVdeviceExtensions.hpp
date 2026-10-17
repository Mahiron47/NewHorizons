#ifndef HVDEVICEEXTENSIONSHPP
#define HVDEVICEEXTENSIONSHPP

#include "vulkan/device/extensions/hv_device_extension_list.hpp"

class HVdeviceExtensions : public IHVmodules {
	HVphysicalDevice* _phys_device;
	void init_modules_list() override;

public:
	template<class... Args> requires (std::derived_from<std::remove_reference_t<Args>, IHVmodule> && ...)
	HVdeviceExtensions(HVphysicalDevice* phys_device, Args&&... args) : IHVmodules(std::forward<Args>(args)...), 
																		_phys_device(phys_device) {
		this->initialize();
	}
};

inline void HVdeviceExtensions::init_modules_list() {
	VkResult result;
	uint32_t extensions_count;

	result = vkEnumerateDeviceExtensionProperties(*_phys_device, nullptr, &extensions_count, nullptr);

	if (result != VK_SUCCESS) throw std::runtime_error("HVdeviceExtensions::init_modules_list : failed to enumerate device extension properties.");
	
	VkExtensionProperties* extensions = new VkExtensionProperties[extensions_count];

	result = vkEnumerateDeviceExtensionProperties(*_phys_device, nullptr, &extensions_count, extensions);
	
	if (result != VK_SUCCESS) throw std::runtime_error("HVdeviceExtensions::init_modules_list : failed to enumerate device extension properties.");
	
	this->_modules_list.do_each([](IHVmodule*& val, 
								   uint32_t extensions_count, 
								   VkExtensionProperties* extensions) -> void {
		for (uint32_t i = 0; i < extensions_count; i++) {
			if (strcmp(val->get_module_name(), extensions[i].extensionName) == 0) {
				val->set_accesability(true);
				return;
			}
		}
		throw std::runtime_error("HVdeviceExtensions::init_modules_list : device extension " + 
								 std::string(val->get_module_name()) + " is not supported.");
	}, extensions_count, extensions);

	delete[] extensions;
}

#endif /* HVDEVICEEXTENSIONSHPP */