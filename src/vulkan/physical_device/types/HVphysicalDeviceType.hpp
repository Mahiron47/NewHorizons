#ifndef HPHYSICALDEVICETYPEHPP
#define HPHYSICALDEVICETYPEHPP

#include "vulkan/physical_device/types/support_types/HVphysicalDeviceVulkanSupportType.hpp"
#include "vulkan/physical_device/types/support_types/HVphysicalDeviceEfficiencyType.hpp"

inline constexpr uint32_t operator|(EHVsupportType lhs, EHVefficiencyType rhs) noexcept {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}
inline constexpr uint32_t operator|(EHVefficiencyType lhs, EHVsupportType rhs) noexcept {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

template<class... Args> requires ((requires(uint32_t t, VkPhysicalDeviceProperties properties) { 
    { Args(t).is_suitable(properties) } -> std::same_as<bool>; 
}) && ...) class HVphysicalDeviceType {
    uint32_t _type;

public:
    constexpr HVphysicalDeviceType(uint32_t type) noexcept : _type(type) {}

    constexpr bool is_suitable(const VkPhysicalDeviceProperties& properties) const noexcept {
        return (Args(_type).is_suitable(properties) && ...);
    }
};

#endif // HPHYSICALDEVICETYPEHPP