#ifndef HVDEVICEEXTENSIONLISTHPP
#define HVDEVICEEXTENSIONLISTHPP

#define IHV_MODULE_DEVICE_EXTENSION(extension_name, prefix, extension_name_macros) \
    class HVextension##prefix####extension_name : public IHVmodule { \
    public: \
        const char* get_module_name() const override { \
            return VK_##prefix##_##extension_name_macros##_EXTENSION_NAME; \
        } \
        IHVmodule* initialize() const override { \
            return new HVextension##prefix####extension_name(); \
        } \
    };

IHV_MODULE_DEVICE_EXTENSION(swapchain, KHR, SWAPCHAIN)
IHV_MODULE_DEVICE_EXTENSION(maintenance1, KHR, MAINTENANCE1)
IHV_MODULE_DEVICE_EXTENSION(maintenance2, KHR, MAINTENANCE2)
IHV_MODULE_DEVICE_EXTENSION(maintenance3, KHR, MAINTENANCE3)
IHV_MODULE_DEVICE_EXTENSION(descriptorIndexing, EXT, DESCRIPTOR_INDEXING)
IHV_MODULE_DEVICE_EXTENSION(bufferDeviceAddress, KHR, BUFFER_DEVICE_ADDRESS)
IHV_MODULE_DEVICE_EXTENSION(scalarBlockLayout, EXT, SCALAR_BLOCK_LAYOUT)
IHV_MODULE_DEVICE_EXTENSION(uniformBufferStandardLayout, KHR, UNIFORM_BUFFER_STANDARD_LAYOUT)
IHV_MODULE_DEVICE_EXTENSION(inlineUniformBlock, EXT, INLINE_UNIFORM_BLOCK)
IHV_MODULE_DEVICE_EXTENSION(pushDescriptor, KHR, PUSH_DESCRIPTOR)
IHV_MODULE_DEVICE_EXTENSION(dynamicRendering, KHR, DYNAMIC_RENDERING)
IHV_MODULE_DEVICE_EXTENSION(synchronization2, KHR, SYNCHRONIZATION_2)
IHV_MODULE_DEVICE_EXTENSION(timelineSemaphore, KHR, TIMELINE_SEMAPHORE)
IHV_MODULE_DEVICE_EXTENSION(copyCommands2, KHR, COPY_COMMANDS_2)

#endif