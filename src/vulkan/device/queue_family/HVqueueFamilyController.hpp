#ifndef HVQUEUEFAMILYCONTROLLERHPP
#define HVQUEUEFAMILYCONTROLLERHPP
    
#include "vulkan/device/queue_family/instance/HVqueueFamily.hpp"

class HVqueueFamilyController {
    Hstack<uint8_t, HVqueueFamily*> _queue_families;
    IHVqueueFamiliesSetStrategy*    _strategy;

    HVqueueFamilyController(IHVqueueFamiliesSetStrategy* strategy,  
                            Hstack<uint8_t, HVqueueFamily*>&& queue_families) : _strategy(strategy), 
                                                                                _queue_families(std::move(queue_families)) {}

public:
    static HVqueueFamilyController* initialize(VkDevice device, IHVqueueFamiliesSetStrategy* strategy) {
        return new HVqueueFamilyController(strategy, strategy->create_queue_families_stack(device));
    }

    HVqueueFamily* get_queue_family(uint8_t index) const noexcept {
        return this->_queue_families[index];
    }
    constexpr uint8_t get_queue_family_count() const noexcept { return this->_queue_families.size(); }

   ~HVqueueFamilyController() {
        for (uint8_t i = 0; i < _queue_families.size(); i++) {
            delete _queue_families[i];
        }
        delete _strategy;
    }
    HVqueueFamilyController(HVqueueFamilyController&& other) noexcept : _queue_families(std::move(other._queue_families)) {}
    HVqueueFamilyController& operator=(HVqueueFamilyController&& other) noexcept {
        if (this == &other) return *this;
        
        _queue_families = std::move(other._queue_families);

        return *this;
    }
    HVqueueFamilyController(const HVqueueFamilyController& other) = delete;
    HVqueueFamilyController& operator=(const HVqueueFamilyController& other) = delete;

};


 
#endif // HVQUEUEFAMILYCONTROLLERHPP