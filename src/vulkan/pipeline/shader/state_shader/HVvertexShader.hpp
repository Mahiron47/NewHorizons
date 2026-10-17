#ifndef HVVERTEXSHADERHPP
#define HVVERTEXSHADERHPP

class HVvertexShader {
    HVshaderModule* _shader_module;

    HVvertexShader(HVshaderModule* shader_module) : _shader_module(shader_module) {}
public:
    static HVvertexShader* initialize(HVdevice* device, const char* shader_path);

    operator VkShaderModule() const { return *_shader_module; }

   ~HVvertexShader() {
        if (_shader_module) delete _shader_module;
    }
    HVvertexShader(HVvertexShader&& other) noexcept : _shader_module(other._shader_module) {
        other._shader_module = nullptr;
    }
    HVvertexShader& operator=(HVvertexShader&& other) noexcept {
        if (this == &other) return *this;

        this->_shader_module = other._shader_module;

        other._shader_module = nullptr;

        return *this;
    }
    HVvertexShader(const HVvertexShader&) = delete;
    HVvertexShader& operator=(const HVvertexShader&) = delete;
};

inline HVvertexShader* HVvertexShader::initialize(HVdevice* device, const char* shader_path) {
    HVshaderModule* shader_module = HVshaderModule::initialize(device, shader_path);

    VkPipelineShaderStageCreateInfo shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = *shader_module,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    return new HVvertexShader(shader_module);
}

#endif // HVVERTEXSHADERHPP