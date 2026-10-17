#ifndef HVDEVICEHPP
#define HVDEVICEHPP

#include "vulkan/physical_device/HVphysicalDeviceController.hpp"
#include "vulkan/device/queue_family/strategy/IHVqueueFamiliesSetStrategy.hpp"

#include "vulkan/device/queue_family/strategy/realizations/HVgraphicSet1.hpp"

#include "vulkan/device/queue_family/HVqueueFamilyController.hpp"
#include "vulkan/device/extensions/HVdeviceExtensions.hpp"
#include "vulkan/device/features/HVdeviceFeatures.hpp"

class HVdevice {
    VkDevice _device;

    HVqueueFamilyController*  _queue_family_controller;
    const HVdeviceExtensions* _extensions;
    const HVdeviceFeatures*   _features;

    HVdevice(VkDevice device,
             HVqueueFamilyController* queue_family_controller,
             HVdeviceExtensions* extensions) : _device(device),
                                               _queue_family_controller(queue_family_controller),
                                               _extensions(extensions),
                                               _features(nullptr) {}

    HVdevice(VkDevice device,
             HVqueueFamilyController* queue_family_controller,
             HVdeviceExtensions* extensions,
             HVdeviceFeatures* features) : _device(device),
                                           _queue_family_controller(queue_family_controller),
                                           _extensions(extensions),
                                           _features(features) {}

public:
    static HVdevice* initialize(HVphysicalDevice* phys_device,
                                HVinstance* instance, 
                                IHVqueueFamiliesSetStrategy* queue_strategy,
                                HVdeviceExtensions&& extensions);

    static HVdevice* initialize(HVphysicalDevice* phys_device,
                                HVinstance* instance, 
                                IHVqueueFamiliesSetStrategy* queue_strategy,
                                HVdeviceExtensions&& extensions,
                                HVdeviceFeatures&& features);

    constexpr HVqueueFamilyController* get_queue_family_controller() const noexcept { return _queue_family_controller; }
    constexpr const HVdeviceExtensions* get_extensions() const noexcept { return _extensions; }
    constexpr const HVdeviceFeatures* get_features() const noexcept { return _features; }
    
    constexpr operator VkDevice() const { return _device; }
    
   ~HVdevice() {
        if (_queue_family_controller) delete _queue_family_controller;
        if (_extensions) delete _extensions;
        if (_features) delete _features;

        if (_device != VK_NULL_HANDLE) vkDestroyDevice(_device, nullptr);
    }
    HVdevice(HVdevice&& other) noexcept : _device(other._device),
                                          _queue_family_controller(other._queue_family_controller),
                                          _extensions(other._extensions),
                                          _features(other._features) {
        other._device = VK_NULL_HANDLE;
        other._queue_family_controller = VK_NULL_HANDLE;
        other._extensions = nullptr;
        other._features = nullptr;
    }
    HVdevice& operator=(HVdevice&& other) noexcept {
        if (this == &other) return *this;

        this->_device = other._device;
        this->_queue_family_controller = other._queue_family_controller;
        this->_extensions = other._extensions;
        this->_features = other._features;

        other._device = VK_NULL_HANDLE;
        other._queue_family_controller = VK_NULL_HANDLE;
        other._extensions = nullptr;
        other._features = nullptr;

        return *this;
    }
    HVdevice(const HVdevice&) = delete;
    HVdevice& operator=(const HVdevice&) = delete;

};

inline HVdevice* HVdevice::initialize(HVphysicalDevice* phys_device, 
                                      HVinstance* instance,
                                      IHVqueueFamiliesSetStrategy* queue_strategy,
                                      HVdeviceExtensions&& extensions) {
    VkDevice device;

    /* DEVICE CREATION */ {
        VkResult result;
    
        HVinitialQueueFamiliesPackage queue_infos_package = queue_strategy->get_initial_queue_families();
        VkPhysicalDeviceFeatures features = {
            // VK_FALSE inicialization
        };

        VkDeviceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = queue_infos_package.queue_infos->size(),
            .pQueueCreateInfos = queue_infos_package.queue_infos->data(),
            .enabledLayerCount = 0,         // deprecated
            .ppEnabledLayerNames = nullptr, // deprecated 
            .enabledExtensionCount = extensions.get_size(),
            .ppEnabledExtensionNames = extensions.get_data(),
            .pEnabledFeatures = &features
        };

        result = vkCreateDevice(*phys_device, &create_info, nullptr, &device);

        if (result != VK_SUCCESS) throw std::runtime_error("HVdevice::initialize : failed to create device.");
    } /* DEVICE CREATION */

    return new HVdevice(device, HVqueueFamilyController::initialize(device, queue_strategy),
                        new HVdeviceExtensions(std::move(extensions)));
}

inline HVdevice* HVdevice::initialize(HVphysicalDevice* phys_device,
                                      HVinstance* instance, 
                                      IHVqueueFamiliesSetStrategy* queue_strategy,
                                      HVdeviceExtensions&& extensions,
                                      HVdeviceFeatures&& features) {
    VkDevice device;

    /* DEVICE CREATION */ {
        VkResult result;

        HVinitialQueueFamiliesPackage queue_infos_package = queue_strategy->get_initial_queue_families();
        uint32_t vulkan_version = instance->get_vulkan_version();

        VkPhysicalDeviceFeatures features1;
        HVdeviceFeaturesPackage features_pack;

        if (vulkan_version == VK_API_VERSION_1_0) {
            features1 = features.get_features();
        } else {
            features_pack = features.get_vulkan_features(instance);
        }

        VkDeviceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = vulkan_version == VK_API_VERSION_1_0 ? nullptr : &features_pack.features2,
            .flags = 0,
            .queueCreateInfoCount = queue_infos_package.queue_infos->size(),
            .pQueueCreateInfos = queue_infos_package.queue_infos->data(),
            .enabledLayerCount = 0,         // deprecated
            .ppEnabledLayerNames = nullptr, // deprecated 
            .enabledExtensionCount = extensions.get_size(),
            .ppEnabledExtensionNames = extensions.get_data(),
            .pEnabledFeatures = vulkan_version == VK_API_VERSION_1_0 ? &features1 : nullptr
        };

        result = vkCreateDevice(*phys_device, &create_info, nullptr, &device);

        if (result != VK_SUCCESS) throw std::runtime_error("HVdevice::initialize : failed to create device.");
    } /* DEVICE CREATION */

    return new HVdevice(device, HVqueueFamilyController::initialize(device, queue_strategy),
                        new HVdeviceExtensions(std::move(extensions)),
                        new HVdeviceFeatures(std::move(features)));
}

#endif /* HVDEVICEHPP */