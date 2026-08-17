#include "HVphysicalDevice.hpp"

HVphysicalDevice* HVphysicalDevice::initialize(const HVinstance* instance) {
	Hstack<uint32_t, VkPhysicalDevice> physical_devices_stack;

    /* Physical device enumerating */ {
		uint32_t physical_device_count;
		VkResult result = vkEnumeratePhysicalDevices(*instance, &physical_device_count, nullptr);
		
		if (result != VK_SUCCESS || physical_device_count == 0) 
			throw std::runtime_error("HVphysicalDevice:initialize : Failed to find GPUs with Vulkan support!");

		VkPhysicalDevice* physical_devices = new VkPhysicalDevice[physical_device_count]; 

		result = vkEnumeratePhysicalDevices(*instance, &physical_device_count, physical_devices);
		
		if (result != VK_SUCCESS) 
			throw std::runtime_error("HVphysicalDevice:initialize : Failed to enumerate physical devices!");

		for (uint32_t i = 0; i < physical_device_count; i++) {
			physical_devices_stack.push(physical_devices[i]);
		}

		delete[] physical_devices; 
	} /* Physical device enumerating */

	return new HVphysicalDevice(std::move(physical_devices_stack));
}

VkPhysicalDevice HVphysicalDevice::pickDevice(const HphysicalDeviceType& type) {
	VkPhysicalDevice* device = nullptr;

	_physical_devices.do_each([](VkPhysicalDevice& device_i, const HphysicalDeviceType& type) -> bool {
		return type.isSuitable(device_i);
	}, &device, type);

	return device != nullptr ? *device : VK_NULL_HANDLE;
}