#ifndef HPHYSICALDEVICEVULKANSUPPORTTYPEHPP
#define HPHYSICALDEVICEVULKANSUPPORTTYPEHPP

enum class EHVsupportType : uint32_t {
	VK_VER_1_0 = 0b000'000,
	VK_VER_1_1 = 0b001'000,
	VK_VER_1_2 = 0b010'000,
	VK_VER_1_3 = 0b011'000,
	VK_VER_1_4 = 0b100'000
};

inline constexpr EHVsupportType operator|(EHVsupportType lhs, EHVsupportType rhs) noexcept {
	return EHVsupportType(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

class HVphysicalDeviceVulkanSupportType {
    const EHVsupportType _type;

public:
	constexpr HVphysicalDeviceVulkanSupportType(uint32_t type) : _type(EHVsupportType {type & 0b111'000}) {}

    constexpr bool is_suitable(const VkPhysicalDeviceProperties& properties) const noexcept {
        switch (this->_type) { 
		case EHVsupportType::VK_VER_1_0: 
            return VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                   VK_VERSION_MINOR(properties.apiVersion) >= 0);
        case EHVsupportType::VK_VER_1_1: 
            return VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                   VK_VERSION_MINOR(properties.apiVersion) >= 1);
        case EHVsupportType::VK_VER_1_2:
            return VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                   VK_VERSION_MINOR(properties.apiVersion) >= 2);
        case EHVsupportType::VK_VER_1_3:
            return VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                   VK_VERSION_MINOR(properties.apiVersion) >= 3);
        case EHVsupportType::VK_VER_1_4:
            return VK_VERSION_MAJOR(properties.apiVersion) > 1 || (VK_VERSION_MAJOR(properties.apiVersion) == 1 && 
                   VK_VERSION_MINOR(properties.apiVersion) >= 4);
        };

        return false;
    }
};

#endif // HPHYSICALDEVICEVULKANSUPPORTTYPEHPP