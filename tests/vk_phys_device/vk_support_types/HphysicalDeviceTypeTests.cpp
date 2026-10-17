#include <catch2/catch_test_macros.hpp>

#include "Horizon.hpp"

TEST_CASE("HphysicalDeviceType::is_suitable", "[HphysicalDeviceType]") {
	HVinstance* instance = HVinstance::initialize();
	HVphysicalDeviceController* devices = HVphysicalDeviceController::initialize(instance);

	VkPhysicalDeviceProperties properties;
	
	/* Physical device enumerating */ {
		Hstack<uint32_t, VkPhysicalDevice> physical_devices_stack;
		uint32_t physical_device_count;
		
		VkResult result = vkEnumeratePhysicalDevices(*instance, &physical_device_count, nullptr);
		
		if (result != VK_SUCCESS || physical_device_count == 0) throw std::runtime_error("HVphysicalDevice:initialize : Failed to find GPUs with Vulkan support!");

		VkPhysicalDevice* physical_devices = new VkPhysicalDevice[physical_device_count]; 

		result = vkEnumeratePhysicalDevices(*instance, &physical_device_count, physical_devices);
		
		if (result != VK_SUCCESS) throw std::runtime_error("HVphysicalDevice:initialize : Failed to enumerate physical devices!");

		for (uint32_t i = 0; i < physical_device_count; i++) {
			physical_devices_stack.push(physical_devices[i]);
		}
		
		delete[] physical_devices; 

		vkGetPhysicalDeviceProperties(physical_devices_stack.last(), &properties);
	} /* Physical device enumerating */

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_0, BEST_PERFORMANCE : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_0 | 
										   									EHVefficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_0, BEST_POWER_EFFICIENCY : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_0 | 
										   									EHVefficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_1, BEST_PERFORMANCE : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_1 | 
										   									EHVefficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_1, BEST_POWER_EFFICIENCY : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_1 | 
										   									EHVefficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_2, BEST_PERFORMANCE : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_2 | 
										   									EHVefficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_2, BEST_POWER_EFFICIENCY : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_2 | 
										   									EHVefficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_3, BEST_PERFORMANCE : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_3 | 
										   									EHVefficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_3, BEST_POWER_EFFICIENCY : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_3 | 
										   									EHVefficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_4, BEST_PERFORMANCE : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_4 | 
										   									EHVefficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	SECTION("HVphysicalDeviceType::is_suitable ; VK_VER_1_4, BEST_POWER_EFFICIENCY : simple check") {
		HVphysicalDeviceType<HVphysicalDeviceVulkanSupportType, 
							 HVphysicalDeviceEfficiencyType> deviceType = { EHVsupportType::VK_VER_1_4 | 
										   									EHVefficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.is_suitable(properties) == func());
	}

	delete devices;
	delete instance;
}