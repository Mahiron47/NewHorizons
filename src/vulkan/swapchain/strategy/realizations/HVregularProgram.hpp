#ifndef HVREGULARPROGRAMHPP
#define HVREGULARPROGRAMHPP

class HVregularProgram : public IHVswapchainConfigurationStrategy {
    uint32_t get_min_image_count() const override {
        return 2;
    }
    VkFormat get_image_format() const override {
        return VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
    }
    VkColorSpaceKHR get_color_space() const override {
        return VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    }
    VkSurfaceTransformFlagBitsKHR get_pre_transform() const override {
        return VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    VkCompositeAlphaFlagBitsKHR get_composite_alpha() const override {
        return VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    }
    VkPresentModeKHR get_present_mode() const override {
        return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    }
};

#endif // HVREGULARPROGRAMHPP