#ifndef HVDYNAMICSTATEHPP
#define HVDYNAMICSTATEHPP

class HVdynamicState {
public:
    static constexpr VkPipelineDynamicStateCreateInfo initialize() {
        return VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr
        };
    }

};

#endif // HVDYNAMICSTATEHPP