#include <catch2/catch_test_macros.hpp>

#include "Horizon.hpp"

TEST_CASE("HVphysicalDevice::initialize", "[HVphysicalDevice]") {
    SECTION("HVphysicalDevice::initialize ; HVinstance::initialize : simple check") {
        HVinstance* instance = HVinstance::initialize();
        HVphysicalDeviceController* devices = HVphysicalDeviceController::initialize(instance);

        REQUIRE(devices != nullptr);
        REQUIRE(Hstack<uint32_t, HVphysicalDevice*> { *devices }.size() > 0);

        delete devices;
        delete instance;
    }
}

TEST_CASE("HVphysicalDevice::pick_device", "[HVphysicalDevice]") {
    HVinstance* instance = HVinstance::initialize();
    HVphysicalDeviceController* devices = HVphysicalDeviceController::initialize(instance);

    SECTION("HVphysicalDevice::pick_device ; HVphysicalDeviceType::is_suitable, HVinstance::initialize, HVphysicalDevice::initialize : EHVefficiencyType::BEST_PERFORMANCE simple check") {
        HVphysicalDevice* device1 = devices->pick_device(EHVsupportType::VK_VER_1_0 | EHVefficiencyType::BEST_PERFORMANCE);
        
        Hstack<uint32_t, HVphysicalDevice*> physical_devices = *devices;
        HVphysicalDevice** device2 = nullptr;

        physical_devices.do_each([](HVphysicalDevice*& device_i) -> bool {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(*device_i, &properties);

            if ((VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                VK_VERSION_MINOR(properties.apiVersion) >= 0)) && (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) 
                return true;

            return false;
        }, &device2);

        REQUIRE(device1 == (device2 != nullptr ? *device2 : VK_NULL_HANDLE));

        if (device2 == nullptr) return;

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(**device2, &properties);

        REQUIRE((VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                                                                VK_VERSION_MINOR(properties.apiVersion) >= 0)));
        REQUIRE(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);

        vkGetPhysicalDeviceProperties(*device1, &properties);

        REQUIRE((VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                                                                VK_VERSION_MINOR(properties.apiVersion) >= 0)));
        REQUIRE(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    }

    SECTION("HVphysicalDevice::pick_device ; HVphysicalDeviceType::is_suitable, HVinstance::initialize, HVphysicalDevice::initialize : EHVefficiencyType::BEST_POWER_EFFICIENCY simple check") {
        HVphysicalDevice* device1 = devices->pick_device(EHVsupportType::VK_VER_1_0 | EHVefficiencyType::BEST_POWER_EFFICIENCY);
        
        Hstack<uint32_t, HVphysicalDevice*> physical_devices = *devices;
        HVphysicalDevice** device2 = nullptr;

        physical_devices.do_each([](HVphysicalDevice*& device_i) -> bool {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(*device_i, &properties);

            if ((VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                 VK_VERSION_MINOR(properties.apiVersion) >= 0)) && (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)) 
                return true;

            return false;
        }, &device2);

        REQUIRE(device1 == (device2 != nullptr ? *device2 : VK_NULL_HANDLE));

        if (device2 == nullptr) return;

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(**device2, &properties);

        REQUIRE((VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                                                                 VK_VERSION_MINOR(properties.apiVersion) >= 0)));
        REQUIRE(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);

        vkGetPhysicalDeviceProperties(*device1, &properties);

        REQUIRE((VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                                                                 VK_VERSION_MINOR(properties.apiVersion) >= 0)));
        REQUIRE(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
    }

    delete devices;
    delete instance;
}