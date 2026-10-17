#ifndef HVPHYSICALDEVICECONTROLLERHPP
#define HVPHYSICALDEVICECONTROLLERHPP

#include "vulkan/physical_device/types/HVphysicalDeviceType.hpp"
#include "vulkan/physical_device/instance/HVphysicalDevice.hpp"

class HVphysicalDeviceController {
	Hstack<uint32_t, HVphysicalDevice*> _physical_devices;

	HVphysicalDeviceController(Hstack<uint32_t, HVphysicalDevice*>&& physical_devices) : _physical_devices(physical_devices) {}

public:
	static HVphysicalDeviceController* initialize(const HVinstance* instance);
	
	HVphysicalDevice* pick_device(const HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
                                                             HVphysicalDeviceEfficiencyType>& type) noexcept;

	operator Hstack<uint32_t, HVphysicalDevice*>() const { return _physical_devices; }

	HVphysicalDeviceController(HVphysicalDeviceController&& other) noexcept : _physical_devices(std::move(other._physical_devices)) {}
    HVphysicalDeviceController& operator=(HVphysicalDeviceController&& other) noexcept {
        if (this != &other) return *this;

        this->_physical_devices = std::move(other._physical_devices);

        return *this;
    }
    HVphysicalDeviceController(const HVphysicalDeviceController&) = delete;
	HVphysicalDeviceController& operator=(const HVphysicalDeviceController&) = delete;

   ~HVphysicalDeviceController() {
		this->_physical_devices.do_each([](HVphysicalDevice*& phys_device) -> void {
			delete phys_device;
		});
    }
};

inline HVphysicalDeviceController* HVphysicalDeviceController::initialize(const HVinstance* instance) {
	Hstack<uint32_t, HVphysicalDevice*> physical_devices_stack;

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
			physical_devices_stack.push(new HVphysicalDevice(physical_devices[i]));
		}

		delete[] physical_devices; 
	} /* Physical device enumerating */

	return new HVphysicalDeviceController(std::move(physical_devices_stack));
}

inline HVphysicalDevice* HVphysicalDeviceController::pick_device(const HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
                                                   		  								    HVphysicalDeviceEfficiencyType>& type) noexcept {
	HVphysicalDevice** device = nullptr;
														
	this->_physical_devices.do_each([](HVphysicalDevice* device_i, 
								 	   const HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
                                 	                              HVphysicalDeviceEfficiencyType>& type) -> bool {
		VkPhysicalDeviceProperties properties;
    	vkGetPhysicalDeviceProperties(*device_i, &properties);

		return type.is_suitable(properties);
	}, &device, type);

	return device != nullptr ? *device : nullptr;
}

#endif // HVPHYSICALDEVICECONTROLLERHPP