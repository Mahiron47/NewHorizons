#ifndef HVINSTANCEEXTENSIONLISTHPP
#define HVINSTANCEEXTENSIONLISTHPP

#define IHV_MODULE_INSTANCE_EXTENSION(extension_name, prefix, extension_name_macros) \
    class HVextension##prefix####extension_name : public IHVmodule { \
    public: \
        const char* get_module_name() const override { \
            return VK_##prefix##_##extension_name_macros##_EXTENSION_NAME; \
        } \
        IHVmodule* initialize() const override { \
            return new HVextension##prefix####extension_name(); \
        } \
    };

IHV_MODULE_INSTANCE_EXTENSION(surface, KHR, SURFACE)
#ifdef VK_USE_PLATFORM_WIN32_KHR
IHV_MODULE_INSTANCE_EXTENSION(WIN32surface, KHR, WIN32_SURFACE)
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
IHV_MODULE_INSTANCE_EXTENSION(XCBsurface, KHR, XCB_SURFACE)
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR 
IHV_MODULE_INSTANCE_EXTENSION(waylandSurface, KHR, WAYLAND_SURFACE)
#endif
IHV_MODULE_INSTANCE_EXTENSION(debugUtils, EXT, DEBUG_UTILS)

#endif // HVINSTANCEEXTENSIONLISTHPP