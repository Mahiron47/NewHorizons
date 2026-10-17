#ifndef HVPIPELINEHPP
#define HVPIPELINEHPP

#include "vulkan/pipeline/shader/HVshaderModule.hpp"

#include "vulkan/pipeline/shader/state_shader/HVvertexShader.hpp"

#include "vulkan/pipeline/state/HVvertexInputState.hpp"
#include "vulkan/pipeline/state/HVinputAssemblyState.hpp"   
#include "vulkan/pipeline/state/HVtessellationState.hpp"
#include "vulkan/pipeline/state/HVviewportState.hpp"
#include "vulkan/pipeline/state/HVrasterizationState.hpp"
#include "vulkan/pipeline/state/HVmultisampleState.hpp"
#include "vulkan/pipeline/state/HVdepthStencilState.hpp"
#include "vulkan/pipeline/state/HVcolorBlendState.hpp"
#include "vulkan/pipeline/state/HVdynamicState.hpp"

class HVpipeline {
    VkPipeline _pipeline;

    HVdevice* _device;

    HVpipeline(VkPipeline pipeline, 
               HVdevice* device) : _pipeline(pipeline), 
                                   _device(device) {}

public:
    static HVpipeline* initialize(HVdevice* device, Hwindow* window);

   ~HVpipeline() {
        if (_pipeline != VK_NULL_HANDLE) vkDestroyPipeline(*_device, _pipeline, nullptr);
    }
    HVpipeline(HVpipeline&& other) noexcept : _pipeline(other._pipeline),
                                              _device(other._device) {}
    HVpipeline& operator=(HVpipeline&& other) noexcept {
        if (this == &other) return *this;

        this->_pipeline = other._pipeline;
        this->_device = other._device;

        other._pipeline = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;

        return *this;
    }
    HVpipeline(const HVpipeline&) = delete;
    HVpipeline& operator=(const HVpipeline&) = delete;
};
 
inline HVpipeline* HVpipeline::initialize(HVdevice* device, Hwindow* window) {
    VkPipeline pipeline;
    VkPipelineCache pipeline_cache;

    /* CREATE_PIPELINE */ { 
        VkResult result;

        /* CHECK CAPABILITIES */ {
            if (!device->get_features()->check_module(HVfeatureMultiDrawIndirect())) throw std::runtime_error("HVpipeline::initialize : device does not support multi draw indirect");
            if (!device->get_features()->check_module(HVfeatureDynamicRendering())) throw std::runtime_error("HVpipeline::initialize : device does not support dynamic rendering");
        } /* CHECK CAPABILITIES */
        
        /* CREATE PIPELINE CACHE */ {
            VkPipelineCacheCreateInfo pipeline_cache_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .initialDataSize = 0,
                .pInitialData = nullptr
            };

            vkCreatePipelineCache(*device, &pipeline_cache_info, nullptr, &pipeline_cache);
        } /* CREATE PIPELINE CACHE */

        VkPipelineVertexInputStateCreateInfo vertex_input_state = HVvertexInputState::initialize();
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state = HVinputAssemblyState::initialize();
        VkPipelineTessellationStateCreateInfo tessellation_state = HVtessellationState::initialize();
        VkPipelineViewportStateCreateInfo viewport_state = HVviewportState::initialize(window);
        VkPipelineRasterizationStateCreateInfo rasterization_state = HVrasterizationState::initialize();
        VkPipelineMultisampleStateCreateInfo multisample_state = HVmultisampleState::initialize();
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state = HVdepthStencilState::initialize();
        VkPipelineColorBlendStateCreateInfo color_blend_state = HVcolorBlendState::initialize();
        VkPipelineDynamicStateCreateInfo dynamic_state = HVdynamicState::initialize();

        VkPipelineLayout pipeline_layout;

        VkPipelineLayoutCreateInfo pipeline_layout_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        vkCreatePipelineLayout(*device, &pipeline_layout_info, nullptr, &pipeline_layout);

        const VkFormat color_attachment_formats[] = {
            VkFormat::VK_FORMAT_B8G8R8A8_SRGB
        };

        VkPipelineRenderingCreateInfo rendering_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = color_attachment_formats,
            .depthAttachmentFormat = VkFormat::VK_FORMAT_D32_SFLOAT,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
        };

        VkGraphicsPipelineCreateInfo pipeline_info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &rendering_info,
            .flags = 0,
            .stageCount = 0,
            .pStages = nullptr,
            .pVertexInputState = &vertex_input_state,
            .pInputAssemblyState = &input_assembly_state,
            .pTessellationState = &tessellation_state,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_state,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = &depth_stencil_state,
            .pColorBlendState = &color_blend_state,
            .pDynamicState = &dynamic_state,
            .layout = pipeline_layout,
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        result = vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline);
    
        if (result != VK_SUCCESS) throw std::runtime_error("HVpipeline::initialize : failed to create graphics pipeline");
    } /* CREATE_PIPELINE */
 
    return new HVpipeline(pipeline, device);
}

#endif // HVPIPELINEHPP