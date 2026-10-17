#ifndef HVTESSELLATIONSTATEHPP
#define HVTESSELLATIONSTATEHPP

class HVtessellationState {
public:
    static constexpr VkPipelineTessellationStateCreateInfo initialize() {
        return VkPipelineTessellationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .patchControlPoints = 3
        };
    }

};

#endif // HVTESSELLATIONSTATEHPP