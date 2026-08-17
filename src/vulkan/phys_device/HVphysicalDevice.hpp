#ifndef HVPHYSICALDEVICEHPP
#define HVPHYSICALDEVICEHPP

#include "memory/Hstack.hpp"
#include "vulkan/instance/HVinstance.hpp"
#include "vulkan/phys_device/HphysicalDeviceVulkanType.hpp"

#include <vulkan/vulkan.hpp>

class HVphysicalDevice {
	Hstack<uint32_t, VkPhysicalDevice> _physical_devices;

	HVphysicalDevice(Hstack<uint32_t, VkPhysicalDevice>&& physical_devices) : _physical_devices(physical_devices) {}

public:
	static HVphysicalDevice* initialize(const HVinstance* instance);
	
	VkPhysicalDevice pickDevice(const HphysicalDeviceType& type);

	operator Hstack<uint32_t, VkPhysicalDevice>() const { return _physical_devices; }

	HVphysicalDevice() : _physical_devices() {}
   ~HVphysicalDevice() = default;

	HVphysicalDevice(HVphysicalDevice&& other) noexcept : _physical_devices(other._physical_devices) {
		other._physical_devices = Hstack<uint32_t, VkPhysicalDevice>();
    }
    HVphysicalDevice& operator=(HVphysicalDevice&& other) noexcept {
        if (this != &other) return *this;

        this->_physical_devices = other._physical_devices;

        other._physical_devices = Hstack<uint32_t, VkPhysicalDevice>();

        return *this;
    }

    HVphysicalDevice(const HVphysicalDevice&) = delete;
	HVphysicalDevice& operator=(const HVphysicalDevice&) = delete;

};

#endif // HVPHYSICALDEVICEHPP