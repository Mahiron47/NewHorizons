#include <catch2/catch_test_macros.hpp>

#include "vulkan/instance/extensions/HVinstanceExtensions.hpp"
#include "vulkan/phys_device/HphysicalDeviceVulkanType.hpp"
#include "vulkan/instance/layers/HVinstanceLayers.hpp"
#include "vulkan/phys_device/HVphysicalDevice.hpp"
#include "vulkan/instance/HVinstance.hpp"

TEST_CASE("HphysicalDeviceVulkanSupportType::isSuitable", "[HphysicalDeviceVulkanSupportType]") {
    HVinstance* instance = HVinstance::initialize(); 
    HVphysicalDevice* devices = HVphysicalDevice::initialize(instance);

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

	SECTION("Should handle VK_VER_1_0 type") {
		HphysicalDeviceVulkanSupportType supportType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_0 };

        auto func = [&properties]() -> bool {
			return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0;
        };

        REQUIRE(supportType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle VK_VER_1_1 type") {
		HphysicalDeviceVulkanSupportType supportType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_1 };

		auto func = [&properties]() -> bool {
			return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 1;
		};

		REQUIRE(supportType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle VK_VER_1_2 type") {
		HphysicalDeviceVulkanSupportType supportType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_2 };

		auto func = [&properties]() -> bool {
			return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 2;
		};
		
		REQUIRE(supportType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle VK_VER_1_3 type") {
		HphysicalDeviceVulkanSupportType supportType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_3 };
		
		auto func = [&properties]() -> bool {
			return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 3;
		};

		REQUIRE(supportType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle VK_VER_1_4 type") {
		HphysicalDeviceVulkanSupportType supportType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_4 };
		
		auto func = [&properties]() -> bool {
			return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 4;
		};

		REQUIRE(supportType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	delete devices;
	delete instance;
}

TEST_CASE("HphysicalDeviceType::isSuitable", "[HphysicalDeviceType]") {
	HVinstance* instance = HVinstance::initialize();
	HVphysicalDevice* devices = HVphysicalDevice::initialize(instance);

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

	SECTION("Should handle flags for VK_VER_1_0 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_0 | EHphysicalDeviceEfficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_0 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_0 | EHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_1 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_1 | EHphysicalDeviceEfficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_1 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_1 | EHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_2 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_2 | EHphysicalDeviceEfficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_2 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_2 | EHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_3 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_3 | EHphysicalDeviceEfficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_3 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_3 | EHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_4 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_4 | EHphysicalDeviceEfficiencyType::BEST_PERFORMANCE };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	SECTION("Should handle flags for VK_VER_1_4 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType = { EHphysicalDeviceVulkanSupportType::VK_VER_1_4 | EHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY };

		auto func = [&properties]() -> bool {
			return (VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0) &&
				   (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		};

		REQUIRE(deviceType.isSuitable(Hstack<uint32_t, VkPhysicalDevice> { *devices }.last()) == func());
	}

	delete devices;
	delete instance;
}