#ifndef HPHYSICALDEVICEEFFICIENCYTYPEHPP
#define HPHYSICALDEVICEEFFICIENCYTYPEHPP

enum class EHVefficiencyType : uint32_t {
	BEST_PERFORMANCE      = 0b01,
	BEST_POWER_EFFICIENCY = 0b10,
};

inline constexpr EHVefficiencyType operator|(EHVefficiencyType lhs, EHVefficiencyType rhs) noexcept {
	return EHVefficiencyType(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

class HVphysicalDeviceEfficiencyType {
    const EHVefficiencyType _type; 

public:
	constexpr HVphysicalDeviceEfficiencyType(uint32_t type) : _type(EHVefficiencyType {type & 0b11}) {}

    constexpr bool is_suitable(const VkPhysicalDeviceProperties& properties) const noexcept {
        switch (this->_type) { 
        case EHVefficiencyType::BEST_PERFORMANCE:
            return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        case EHVefficiencyType::BEST_POWER_EFFICIENCY:
            return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        };

        return false;
    }
};

#endif // HPHYSICALDEVICEEFFICIENCYTYPEHPP