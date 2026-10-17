#ifndef HVSHADERMODULEHPP
#define HVSHADERMODULEHPP

class HVshaderModule {
    VkShaderModule _shader_module;

    HVdevice* _device;

    HVshaderModule(VkShaderModule shader_module, 
                   HVdevice* device) : _shader_module(shader_module), 
                                       _device(device) {}
public:
    static HVshaderModule* initialize(HVdevice* device, const char* shader_path);

    operator VkShaderModule() const { return _shader_module; }

   ~HVshaderModule() {
        if (_shader_module != VK_NULL_HANDLE) vkDestroyShaderModule(*_device, _shader_module, nullptr);
    }
    HVshaderModule(HVshaderModule&& other) noexcept : _shader_module(other._shader_module),
                                                      _device(other._device) {
        other._shader_module = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }
    HVshaderModule& operator=(HVshaderModule&& other) noexcept {
        if (this == &other) return *this;

        this->_shader_module = other._shader_module;
        this->_device = other._device;

        other._shader_module = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;

        return *this;
    }
    HVshaderModule(const HVshaderModule&) = delete;
    HVshaderModule& operator=(const HVshaderModule&) = delete;
};

inline HVshaderModule* HVshaderModule::initialize(HVdevice* device, const char* shader_path) {
    VkShaderModule shader_module;

    std::ifstream file(shader_path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("HVshaderModule::initialize : failed to open shader file.");
    char* buffer = new char[file.tellg()];
    file.seekg(0, std::ios::beg);
    file.read(buffer, file.tellg());
    file.close();

    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = static_cast<size_t>(file.tellg()),
        .pCode = reinterpret_cast<const uint32_t*>(buffer)
    };

    vkCreateShaderModule(*device, &create_info, nullptr, &shader_module);

    delete[] buffer;
    
    return new HVshaderModule(shader_module, device);
}

#endif // HVSHADERMODULEHPP