#ifndef HORIZONS_HPHYSICALDEVICEVULKANTYPE_HPP
#define HORIZONS_HPHYSICALDEVICEVULKANTYPE_HPP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

#include "memory/Hstack.hpp"
#include "vulkan/instance/HVinstance.hpp"

#include <vulkan/vulkan.hpp>

enum class eHphysicalDeviceVulkanSupportType {
	VK_VER_1_0 = 0b000'000,
	VK_VER_1_1 = 0b001'000,
	VK_VER_1_2 = 0b010'000,
	VK_VER_1_3 = 0b011'000,
	VK_VER_1_4 = 0b100'000
};

enum class eHphysicalDeviceEfficiencyType {
	BEST_PERFORMANCE      = 0b01,
	BEST_POWER_EFFICIENCY = 0b10,
};

inline eHphysicalDeviceVulkanSupportType operator|(eHphysicalDeviceVulkanSupportType lhs, eHphysicalDeviceVulkanSupportType rhs) {
	return eHphysicalDeviceVulkanSupportType(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline eHphysicalDeviceEfficiencyType operator|(eHphysicalDeviceEfficiencyType lhs, eHphysicalDeviceEfficiencyType rhs) {
	return eHphysicalDeviceEfficiencyType(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline uint32_t operator|(eHphysicalDeviceVulkanSupportType lhs, eHphysicalDeviceEfficiencyType rhs) {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

inline uint32_t operator|(eHphysicalDeviceEfficiencyType lhs, eHphysicalDeviceVulkanSupportType rhs) {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

template<typename eHphysicalDeviceVulkanType> requires requires(eHphysicalDeviceVulkanType T) {
	{ ((uint32_t) T) } -> std::convertible_to<uint32_t>;
} class HphysicalDeviceVulkanType {
protected:
	const eHphysicalDeviceVulkanType type;

public:
	HphysicalDeviceVulkanType(eHphysicalDeviceVulkanType type) : type(type) {}

	virtual bool isSuitable(VkPhysicalDevice device) const = 0;
};

class HphysicalDeviceEfficiencyType : protected HphysicalDeviceVulkanType<eHphysicalDeviceEfficiencyType> {
public:
	HphysicalDeviceEfficiencyType(eHphysicalDeviceEfficiencyType type) : HphysicalDeviceVulkanType<eHphysicalDeviceEfficiencyType>(type) {}

    bool isSuitable(VkPhysicalDevice device) const {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        switch (this->type) { 
        case eHphysicalDeviceEfficiencyType::BEST_PERFORMANCE:
            return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        case eHphysicalDeviceEfficiencyType::BEST_POWER_EFFICIENCY:
            return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        };

        return false;
    }
};

class HphysicalDeviceVulkanSupportType : protected HphysicalDeviceVulkanType<eHphysicalDeviceVulkanSupportType> {
public:
	HphysicalDeviceVulkanSupportType(eHphysicalDeviceVulkanSupportType type) : HphysicalDeviceVulkanType<eHphysicalDeviceVulkanSupportType>(type) {}

    bool isSuitable(VkPhysicalDevice device) const {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        switch (this->type) { 
		case eHphysicalDeviceVulkanSupportType::VK_VER_1_0: 
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 0;
        case eHphysicalDeviceVulkanSupportType::VK_VER_1_1: 
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 1;
        case eHphysicalDeviceVulkanSupportType::VK_VER_1_2:
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 2;
        case eHphysicalDeviceVulkanSupportType::VK_VER_1_3:
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 3;
        case eHphysicalDeviceVulkanSupportType::VK_VER_1_4:
            return VK_VERSION_MAJOR(properties.apiVersion) >= 1 && VK_VERSION_MINOR(properties.apiVersion) >= 4;
        };

        return false;
    }
};

class HphysicalDeviceType : protected HphysicalDeviceVulkanSupportType, 
                            protected HphysicalDeviceEfficiencyType {
public:
    HphysicalDeviceType(uint32_t type) : HphysicalDeviceVulkanSupportType(eHphysicalDeviceVulkanSupportType {type & 0b111'000} ), 
                                         HphysicalDeviceEfficiencyType(eHphysicalDeviceEfficiencyType {type & 0b000'111} ) {}
    

    bool isSuitable(VkPhysicalDevice device) const {
        return HphysicalDeviceVulkanSupportType::isSuitable(device) && HphysicalDeviceEfficiencyType::isSuitable(device);
    }
};

#pragma GCC diagnostic pop
#endif // HORIZONS_HPHYSICALDEVICEVULKANTYPE_HPP