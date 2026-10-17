#ifndef HVVIEWPORTSTATEHPP
#define HVVIEWPORTSTATEHPP

class HVviewportState {
public:
    static constexpr VkPipelineViewportStateCreateInfo initialize(Hwindow* window) {
        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(window->get_extent().width),
            .height = static_cast<float>(window->get_extent().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor = {
            .offset = { 0, 0 },
            .extent = window->get_extent()
        };
        
        return VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
        };
    }

};

#endif // HVVIEWPORTSTATEHPP