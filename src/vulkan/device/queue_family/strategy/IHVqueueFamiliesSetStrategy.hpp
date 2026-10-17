#ifndef IHVQUEUEFAMILIESSETSTRATEGYHPP
#define IHVQUEUEFAMILIESSETSTRATEGYHPP

#include "vulkan/device/queue_family/instance/HVqueueFamily.hpp"

struct HVqueueFamilyProperties {
    uint32_t family_index;
    VkQueueFamilyProperties properties;
};

struct HVinitialQueueFamiliesPackage {
    const Hstack<uint8_t, VkDeviceQueueCreateInfo>* queue_infos;
    const Hstack<uint8_t, const float*>* queue_infos_priorities;

    ~HVinitialQueueFamiliesPackage() {
        for (uint32_t i = 0; i < queue_infos_priorities->size(); i++) {
            if (queue_infos_priorities->operator[](i)) {
                delete[] queue_infos_priorities->operator[](i);
        }   }
        if (queue_infos_priorities) delete queue_infos_priorities;
        if (queue_infos) delete queue_infos;
    }
    HVinitialQueueFamiliesPackage& operator=(HVinitialQueueFamiliesPackage&&) noexcept = default;
    HVinitialQueueFamiliesPackage& operator=(const HVinitialQueueFamiliesPackage&) = delete;
};

class IHVqueueFamiliesSetStrategy {
protected:
    Hstack<uint8_t, HVqueueFamilyProperties> _properties;
    HVphysicalDevice*                        _phys_device;
    HVsurface*                               _surface;

    // must call in derived class constructor 
    virtual Hstack<uint8_t, HVqueueFamilyProperties> init_algorithm() = 0;

public:
    IHVqueueFamiliesSetStrategy(HVphysicalDevice* phys_device, 
                                HVsurface* surface) : _phys_device(phys_device),
                                                      _surface(surface) {
        uint32_t queue_family_count = 0;
        VkQueueFamilyProperties* queue_family_properties = nullptr;

        /* queue family enumeration */ {
            vkGetPhysicalDeviceQueueFamilyProperties(*phys_device, &queue_family_count, nullptr);

            if (queue_family_count == 0) throw std::runtime_error("HVqueueFamily::initialize : queue_family_count = 0.");

            queue_family_properties = new VkQueueFamilyProperties[queue_family_count];

            vkGetPhysicalDeviceQueueFamilyProperties(*phys_device, &queue_family_count, queue_family_properties);
        } /* queue family enumeration */

        auto properties = Hstack<uint8_t, HVqueueFamilyProperties>(queue_family_count);

        for (uint32_t i = 0; i < queue_family_count; i++) {
            properties[i] = HVqueueFamilyProperties {
                .family_index = i,
                .properties = queue_family_properties[i],
            };
        }

        delete[] queue_family_properties;

        this->_properties = Hstack<uint8_t, HVqueueFamilyProperties>(properties);
    }
    virtual ~IHVqueueFamiliesSetStrategy() = default;

    virtual HVinitialQueueFamiliesPackage get_initial_queue_families() {
        auto queue_infos = Hstack<uint8_t, VkDeviceQueueCreateInfo>(_properties.size());
        auto queue_infos_priorities = Hstack<uint8_t, const float*>(_properties.size());

        for (uint32_t i = 0; i < _properties.size(); i++) {
            float* queue_priorities = new float[_properties[i].properties.queueCount];
            for (uint32_t j = 0; j < _properties[i].properties.queueCount; j++) {
                queue_priorities[j] = 1.0f;
            }
            queue_infos_priorities[i] = queue_priorities;

            queue_infos[i] = VkDeviceQueueCreateInfo {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = _properties[i].family_index,
                .queueCount = _properties[i].properties.queueCount,
                .pQueuePriorities = queue_infos_priorities[i]
            };
        }
        
        return HVinitialQueueFamiliesPackage {
            .queue_infos = new Hstack<uint8_t, VkDeviceQueueCreateInfo>(std::move(queue_infos)),
            .queue_infos_priorities = new Hstack<uint8_t, const float*>(std::move(queue_infos_priorities))
        };
    }
    virtual Hstack<uint8_t, HVqueueFamily*> create_queue_families_stack(VkDevice device) = 0;
};


#endif // IHVQUEUEFAMILIESSETSTRATEGYHPP