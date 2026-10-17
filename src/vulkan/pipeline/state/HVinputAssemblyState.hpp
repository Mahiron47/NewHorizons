#ifndef HVINPUTASSEMBLYSTATEHPP
#define HVINPUTASSEMBLYSTATEHPP

class HVinputAssemblyState {
public:
    static constexpr VkPipelineInputAssemblyStateCreateInfo initialize() {
        return VkPipelineInputAssemblyStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };
    }

};

#endif // HVINPUTASSEMBLYSTATEHPP