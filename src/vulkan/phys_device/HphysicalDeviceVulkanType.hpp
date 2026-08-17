#ifndef HORIZONS_HPHYSICALDEVICEVULKANTYPE_HPP
#define HORIZONS_HPHYSICALDEVICEVULKANTYPE_HPP

#include "vulkan/instance/HVinstance.hpp"
#include "memory/Hstack.hpp"

#include <vulkan/vulkan.hpp>

enum class EHphysicalDeviceVulkanSupportType {
	VK_VER_1_0 = 0b000'000,
	VK_VER_1_1 = 0b001'000,
	VK_VER_1_2 = 0b010'000,
	VK_VER_1_3 = 0b011'000,
	VK_VER_1_4 = 0b100'000
};
enum class EHphysicalDeviceEfficiencyType {
	BEST_PERFORMANCE      = 0b01,
	BEST_POWER_EFFICIENCY = 0b10,
};

inline EHphysicalDeviceVulkanSupportType operator|(EHphysicalDeviceVulkanSupportType lhs, EHphysicalDeviceVulkanSupportType rhs) {
	return EHphysicalDeviceVulkanSupportType(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
inline EHphysicalDeviceEfficiencyType operator|(EHphysicalDeviceEfficiencyType lhs, EHphysicalDeviceEfficiencyType rhs) {
	return EHphysicalDeviceEfficiencyType(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
inline uint32_t operator|(EHphysicalDeviceVulkanSupportType lhs, EHphysicalDeviceEfficiencyType rhs) {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}
inline uint32_t operator|(EHphysicalDeviceEfficiencyType lhs, EHphysicalDeviceVulkanSupportType rhs) {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

template<typename EHphysicalDeviceVulkanType> requires requires(EHphysicalDeviceVulkanType t) {
	{ ( (uint32_t) t ) } -> std::convertible_to<uint32_t>;
} interface HphysicalDeviceVulkanType {
protected:
	const EHphysicalDeviceVulkanType _type;

public:
	HphysicalDeviceVulkanType(EHphysicalDeviceVulkanType type) : _type(type) {}

	virtual bool isSuitable(VkPhysicalDevice device) const = 0;
};

class HphysicalDeviceEfficiencyType : protected HphysicalDeviceVulkanType<EHphysicalDeviceEfficiencyType> {
public:
	HphysicalDeviceEfficiencyType(EHphysicalDeviceEfficiencyType type) : HphysicalDeviceVulkanType<EHphysicalDeviceEfficiencyType>(type) {}

    bool isSuitable(VkPhysicalDevice device) const {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        switch (this->_type) { 
        case EHphysicalDeviceEfficiencyType::BEST_PERFORMANCE:
            return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        case EHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY:
            return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        };

        return false;
    }
};

class HphysicalDeviceVulkanSupportType : protected HphysicalDeviceVulkanType<EHphysicalDeviceVulkanSupportType> {
public:
	HphysicalDeviceVulkanSupportType(EHphysicalDeviceVulkanSupportType type) : HphysicalDeviceVulkanType<EHphysicalDeviceVulkanSupportType>(type) {}

    bool isSuitable(VkPhysicalDevice device) const {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        switch (this->_type) { 
		case EHphysicalDeviceVulkanSupportType::VK_VER_1_0: 
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0;
        case EHphysicalDeviceVulkanSupportType::VK_VER_1_1: 
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 1;
        case EHphysicalDeviceVulkanSupportType::VK_VER_1_2:
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 2;
        case EHphysicalDeviceVulkanSupportType::VK_VER_1_3:
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 3;
        case EHphysicalDeviceVulkanSupportType::VK_VER_1_4:
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 4;
        };

        return false;
    }
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

class HphysicalDeviceType : protected HphysicalDeviceVulkanSupportType, 
                            protected HphysicalDeviceEfficiencyType {
public:
    HphysicalDeviceType(uint32_t type) : HphysicalDeviceVulkanSupportType(EHphysicalDeviceVulkanSupportType { type  & 0b111'000 } ), 
                                         HphysicalDeviceEfficiencyType(EHphysicalDeviceEfficiencyType { type  & 0b000'111 } ) {}
    
    bool isSuitable(VkPhysicalDevice device) const {
        return HphysicalDeviceVulkanSupportType::isSuitable(device) && HphysicalDeviceEfficiencyType::isSuitable(device);
    }
};

#pragma GCC diagnostic pop
#endif // HORIZONS_HPHYSICALDEVICEVULKANTYPE_HPP