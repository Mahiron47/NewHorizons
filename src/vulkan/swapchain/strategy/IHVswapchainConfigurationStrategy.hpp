#ifndef HVSWAPCHAINCONFIGURATIONSTRATEGYHPP
#define HVSWAPCHAINCONFIGURATIONSTRATEGYHPP

class IHVswapchainConfigurationStrategy {
public:
    virtual const void* get_swapchain_extension() const {
        return nullptr;
    }
    virtual uint32_t get_min_image_count() const = 0;
    virtual VkFormat get_image_format() const = 0;
    virtual VkColorSpaceKHR get_color_space() const = 0;
    virtual uint32_t image_array_layers() const {
        return 1;
    }
    virtual VkSurfaceTransformFlagBitsKHR get_pre_transform() const = 0;
    virtual VkCompositeAlphaFlagBitsKHR get_composite_alpha() const = 0;
    virtual VkPresentModeKHR get_present_mode() const = 0;
};

#include "vulkan/swapchain/strategy/realizations/HVregularProgram.hpp"
 
#endif // HVSWAPCHAINCONFIGURATIONSTRATEGYHPP