#ifndef HVGRAPHICSET1HPP
#define HVGRAPHICSET1HPP

class HVgraphicSet1 : public IHVqueueFamiliesSetStrategy {
    
    struct TargetFamilies {
        HVqueueFamilyProperties* render_family;
        HVqueueFamilyProperties* transfer_family;
        HVqueueFamilyProperties* compute_family;
    };

    TargetFamilies  _target_families;

public:
    HVgraphicSet1(HVphysicalDevice* phys_device, 
                  HVsurface* surface) : IHVqueueFamiliesSetStrategy(phys_device, surface) {
        this->_properties = this->init_algorithm();
    }

    Hstack<uint8_t, HVqueueFamilyProperties> init_algorithm() override {
        HVqueueFamilyProperties* transfer_graphic_family = nullptr;
        HVqueueFamilyProperties* compute_graphic_family = nullptr;
        HVqueueFamilyProperties* render_family = nullptr;
        HVqueueFamilyProperties* transfer_family = nullptr;
        HVqueueFamilyProperties* compute_family = nullptr;

        for (uint32_t i = 0; i < _properties.size(); i++) {
            VkResult result = VK_RESULT_MAX_ENUM;
            VkBool32 flag = VK_FALSE;

            // check if the queue family supports presentation to the surface
            if (render_family == nullptr) result = vkGetPhysicalDeviceSurfaceSupportKHR(*_phys_device, i, *_surface, &flag);
            if (result != VK_SUCCESS && result != VK_RESULT_MAX_ENUM) throw std::runtime_error("HVgraphicSet1::set_queue_families : failed check surface support.");

            // pick the queue families based on their capabilities and priorities
            if (_properties[i].properties.queueFlags & VK_QUEUE_TRANSFER_BIT && _properties[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT && flag) {
                transfer_graphic_family = &_properties[i];
            }
            if (_properties[i].properties.queueFlags & VK_QUEUE_COMPUTE_BIT && _properties[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT && flag) {
                compute_graphic_family = &_properties[i];
            }
            if (_properties[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT && flag) { 
                render_family = &_properties[i];
            }
            if (_properties[i].properties.queueFlags & VK_QUEUE_TRANSFER_BIT && (_properties[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                transfer_family = &_properties[i];
            }
            if (_properties[i].properties.queueFlags & VK_QUEUE_COMPUTE_BIT && (_properties[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                compute_family = &_properties[i];
            }
        }

        if (render_family != nullptr) {
            _target_families.render_family = render_family; 
        } else {
            throw std::runtime_error("TODO : create transition to another scenery.");
        }

        if (transfer_family != nullptr) {
            _target_families.transfer_family = transfer_family;
        } else if (transfer_family == nullptr && transfer_graphic_family != nullptr) {
            _target_families.transfer_family = transfer_graphic_family;
        } else if (transfer_family == nullptr && transfer_graphic_family == nullptr) {
            throw std::runtime_error("TODO : create transition to another scenery.");
        }


        if (compute_family != nullptr) { 
            _target_families.compute_family = compute_family;        
        } else if (compute_family == nullptr && compute_graphic_family != nullptr) {
            _target_families.compute_family = compute_graphic_family;
        } else if (compute_family == nullptr && compute_graphic_family == nullptr) {
            throw std::runtime_error("TODO : create transition to another scenery.");
        }

        // return the unique queue families that will be used for the device creation
        // 1. check if all families are the same
        if (_target_families.compute_family == _target_families.transfer_family && 
            _target_families.render_family == _target_families.transfer_family) {
            return Hstack<uint8_t, HVqueueFamilyProperties> {
                *_target_families.render_family
            };
        // 2. check if render family unique and compute family unique and transfer family included in render family or compute family
        } else if (_target_families.render_family != _target_families.compute_family && 
                   (_target_families.render_family == _target_families.transfer_family || 
                   _target_families.compute_family == _target_families.transfer_family)) {
            return Hstack<uint8_t, HVqueueFamilyProperties> {
                *_target_families.render_family, *_target_families.compute_family
            };
        // 3. check if render family unique and transfer family unique and compute family included in render family or transfer family
        } else if (_target_families.render_family != _target_families.transfer_family && 
                   (_target_families.render_family == _target_families.compute_family || 
                   _target_families.transfer_family == _target_families.compute_family)) {
            return Hstack<uint8_t, HVqueueFamilyProperties> {
                *_target_families.render_family, *_target_families.transfer_family
            };
        }

        // default solution is to return all three families, if they are unique
        return Hstack<uint8_t, HVqueueFamilyProperties> {
            *_target_families.render_family, *_target_families.transfer_family, *_target_families.compute_family
        };
    }
    Hstack<uint8_t, HVqueueFamily*> create_queue_families_stack(VkDevice device) override {
        VkQueue render_queue1; VkQueue render_queue2;
        
        vkGetDeviceQueue(device, _target_families.render_family->family_index, 0, &render_queue1);
        vkGetDeviceQueue(device, _target_families.render_family->family_index, 1, &render_queue2);

        HVqueueFamily* render_family = new HVqueueFamily(Hstack<uint8_t, VkQueue> {
            render_queue1, render_queue2
        }, _target_families.render_family->family_index, VK_QUEUE_GRAPHICS_BIT, 
           _target_families.render_family->properties.queueCount, 
           _target_families.render_family->properties.minImageTransferGranularity);

        VkQueue transfer_queue;

        vkGetDeviceQueue(device, _target_families.transfer_family->family_index, 0, &transfer_queue);

        HVqueueFamily* transfer_family = new HVqueueFamily(Hstack<uint8_t, VkQueue> {
            transfer_queue
        }, _target_families.transfer_family->family_index, VK_QUEUE_TRANSFER_BIT, 
           _target_families.transfer_family->properties.queueCount, 
           _target_families.transfer_family->properties.minImageTransferGranularity);

        VkQueue compute_queue;

        vkGetDeviceQueue(device, _target_families.compute_family->family_index, 0, &compute_queue);

        HVqueueFamily* compute_family = new HVqueueFamily(Hstack<uint8_t, VkQueue> {
            compute_queue
        }, _target_families.compute_family->family_index, VK_QUEUE_COMPUTE_BIT, 
           _target_families.compute_family->properties.queueCount, 
           _target_families.compute_family->properties.minImageTransferGranularity);
        
        return Hstack<uint8_t, HVqueueFamily*> {
            render_family, transfer_family, compute_family
        };
    }

    HVinitialQueueFamiliesPackage get_initial_queue_families() override {
        auto queue_infos = Hstack<uint8_t, VkDeviceQueueCreateInfo>(_properties.size());
        auto queue_infos_priorities = Hstack<uint8_t, const float*>(_properties.size());
        
        float* render_family_prioritis = new float[2] {
            1.0f, 0.7f
        };
        float* transfer_family_prioritis = new float[1] {
            0.9f
        };
        float* compute_family_prioritis = new float[1] {
            0.5f
        };

        queue_infos_priorities[0] = render_family_prioritis;
        queue_infos_priorities[1] = transfer_family_prioritis;
        queue_infos_priorities[2] = compute_family_prioritis;

        queue_infos[0] = VkDeviceQueueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = _properties[0].family_index,
            .queueCount = 2,
            .pQueuePriorities = queue_infos_priorities[0]
        };
        
        _properties.size() >= 2 ? queue_infos[1] = VkDeviceQueueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = _properties[1].family_index,
            .queueCount = 1,
            .pQueuePriorities = queue_infos_priorities[1]
        } : VkDeviceQueueCreateInfo {};

        _properties.size() >= 3 ? queue_infos[2] = VkDeviceQueueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = _properties[2].family_index,
            .queueCount = 1,
            .pQueuePriorities = queue_infos_priorities[2]
        } : VkDeviceQueueCreateInfo {};
        
        assert(queue_infos.size() <= 3 && "HVgraphicSet1::get_initial_queue_families : queue_infos.size() > 3.");

        return HVinitialQueueFamiliesPackage {
            .queue_infos = new Hstack<uint8_t, VkDeviceQueueCreateInfo>(std::move(queue_infos)),
            .queue_infos_priorities = new Hstack<uint8_t, const float*>(std::move(queue_infos_priorities))
        };
    }
};

#endif // HVGRAPHICSET1HPP