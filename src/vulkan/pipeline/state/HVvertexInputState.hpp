#ifndef HVVERTEXINPUTSTATEHPP
#define HVVERTEXINPUTSTATEHPP

class HVvertexInputState {
    
public:
    static constexpr VkPipelineVertexInputStateCreateInfo initialize() {
        return VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };
    }

};

#endif // HVVERTEXINPUTSTATEHPP