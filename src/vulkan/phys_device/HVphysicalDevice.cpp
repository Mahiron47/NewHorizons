#include "HVphysicalDevice.hpp"

Hstack<uint32_t, HVphysicalDevice> HVphysicalDevice::physicalDevices;

Hstack<uint32_t, HVphysicalDevice>& HVphysicalDevice::initialize(const HVinstance& instance) {
    /* Physical device enumerating */ {
		uint32_t physical_device_count;
		VkResult result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
		if (result != VK_SUCCESS || physical_device_count == 0) {
			throw std::runtime_error("HVphysicalDevice:initialize : Failed to find GPUs with Vulkan support!");
		}

		VkPhysicalDevice* physical_devices = new VkPhysicalDevice[physical_device_count]; // Alocation #1

		result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("HVphysicalDevice:initialize : Failed to enumerate physical devices!");
		}

		for (uint32_t i = 0; i < physical_device_count; i++) {
			HVphysicalDevice::physicalDevices.push(HVphysicalDevice(physical_devices[i]));
		}

		delete[] physical_devices; // Deallocation #1
	} /* Physical device enumerating */

	return HVphysicalDevice::physicalDevices;
}

VkPhysicalDevice HVphysicalDevice::pickDevice(HphysicalDeviceType&& type) {
	HVphysicalDevice* device = nullptr;
	
	auto func = [&type](HVphysicalDevice& device_i) -> bool {
		return type.isSuitable(device_i);
	};

	HVphysicalDevice::physicalDevices.do_each(func, device);

	return device != nullptr ? (VkPhysicalDevice) *device : VK_NULL_HANDLE;
}