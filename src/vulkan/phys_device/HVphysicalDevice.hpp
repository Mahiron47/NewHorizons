#ifndef HV_PHYSICALDEVICE_HPP
#define HV_PHYSICALDEVICE_HPP

#include "memory/Hstack.hpp"
#include "vulkan/instance/HVinstance.hpp"
#include "vulkan/phys_device/HphysicalDeviceVulkanType.hpp"

#include <vulkan/vulkan.hpp>

class HVphysicalDevice {
	static Hstack<uint32_t, HVphysicalDevice> physicalDevices;

	VkPhysicalDevice physicalDevice;

	HVphysicalDevice(VkPhysicalDevice physicalDevice) : physicalDevice(physicalDevice) {}

public:
	static Hstack<uint32_t, HVphysicalDevice>& initialize(const HVinstance& instance);
	
	static VkPhysicalDevice pickDevice(HphysicalDeviceType&& type);

	static VkPhysicalDevice operator[](uint32_t index) { return physicalDevices[index]; }
	operator VkPhysicalDevice() const { return physicalDevice; }

	HVphysicalDevice() : physicalDevice(VK_NULL_HANDLE) {}
   ~HVphysicalDevice() = default;

	HVphysicalDevice(HVphysicalDevice&& other) noexcept : physicalDevice(other.physicalDevice) {
		other.physicalDevice = VK_NULL_HANDLE;
	}
	HVphysicalDevice& operator=(HVphysicalDevice&& other) noexcept {
		if (this != &other) {
			this->physicalDevice = other.physicalDevice;
			other.physicalDevice = VK_NULL_HANDLE;
		}

		return *this;
	}

	HVphysicalDevice(const HVphysicalDevice&) = delete;
	HVphysicalDevice& operator=(const HVphysicalDevice&) = delete;

};

#endif // HV_PHYSICALDEVICE_HPP