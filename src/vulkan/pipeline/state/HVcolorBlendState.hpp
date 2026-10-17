#ifndef HVCOLORBLENDSTATEHPP
#define HVCOLORBLENDSTATEHPP

class HVcolorBlendState {
public:
    static constexpr VkPipelineColorBlendStateCreateInfo initialize() {
        return VkPipelineColorBlendStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 0,
            .pAttachments = nullptr,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
        };
    }

};

#endif // HVCOLORBLENDSTATEHPP