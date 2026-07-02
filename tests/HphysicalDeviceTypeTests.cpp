#include <catch2/catch_test_macros.hpp>

#include "vulkan/phys_device/HphysicalDeviceVulkanType.hpp"
#include "vulkan/phys_device/HVphysicalDevice.hpp"
#include "vulkan/instance/HVinstance.hpp"

TEST_CASE("HphysicalDeviceVulkanSupportType::isSuitable", "[HphysicalDeviceVulkanSupportType]") {
	HVinstance& instance = HVinstance::initialize(true);
	Hstack<uint32_t, HVphysicalDevice>& devices = HVphysicalDevice::initialize(instance);

	SECTION("Should handle VK_VER_1_0 type") {
		HphysicalDeviceVulkanSupportType supportType(eHphysicalDeviceVulkanSupportType::VK_VER_1_0);

		REQUIRE(supportType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle VK_VER_1_1 type") {
		HphysicalDeviceVulkanSupportType supportType(eHphysicalDeviceVulkanSupportType::VK_VER_1_1);
		
		REQUIRE(supportType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle VK_VER_1_2 type") {
		HphysicalDeviceVulkanSupportType supportType(eHphysicalDeviceVulkanSupportType::VK_VER_1_2);
		
		REQUIRE(supportType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle VK_VER_1_3 type") {
		HphysicalDeviceVulkanSupportType supportType(eHphysicalDeviceVulkanSupportType::VK_VER_1_3);
		
		REQUIRE(supportType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle VK_VER_1_4 type") {
		HphysicalDeviceVulkanSupportType supportType(eHphysicalDeviceVulkanSupportType::VK_VER_1_4);
		
		REQUIRE(supportType.isSuitable(devices[0]) == true);
	}
}

TEST_CASE("HphysicalDeviceType::isSuitable", "[HphysicalDeviceType]") {
	HVinstance& instance = HVinstance::initialize(true);
	Hstack<uint32_t, HVphysicalDevice>& devices = HVphysicalDevice::initialize(instance);

	SECTION("Should handle flags for VK_VER_1_0 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_0 | eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE);

		REQUIRE(deviceType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle flags for VK_VER_1_0 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_0 | eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY);

		REQUIRE(deviceType.isSuitable(devices[0]) == false);
	}

	SECTION("Should handle flags for VK_VER_1_1 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_1 | eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE);

		REQUIRE(deviceType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle flags for VK_VER_1_1 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_1 | eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY);

		REQUIRE(deviceType.isSuitable(devices[0]) == false);
	}

	SECTION("Should handle flags for VK_VER_1_2 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_2 | eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE);

		REQUIRE(deviceType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle flags for VK_VER_1_2 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_2 | eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY);

		REQUIRE(deviceType.isSuitable(devices[0]) == false);
	}

	SECTION("Should handle flags for VK_VER_1_3 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_3 | eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE);

		REQUIRE(deviceType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle flags for VK_VER_1_3 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_3 | eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY);

		REQUIRE(deviceType.isSuitable(devices[0]) == false);
	}

	SECTION("Should handle flags for VK_VER_1_4 and BEST_PERFORMANCE") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_4 | eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE);

		HVphysicalDevice::pickDevice(eHphysicalDeviceVulkanSupportType::VK_VER_1_4 | eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE);

		REQUIRE(deviceType.isSuitable(devices[0]) == true);
	}

	SECTION("Should handle flags for VK_VER_1_4 and BEST_POWER_EFFICIENCY") {
		HphysicalDeviceType deviceType(eHphysicalDeviceVulkanSupportType::VK_VER_1_4 | eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY);

		HVphysicalDevice::pickDevice(eHphysicalDeviceVulkanSupportType::VK_VER_1_4 | eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY);

		REQUIRE(deviceType.isSuitable(devices[0]) == false);
	}

}