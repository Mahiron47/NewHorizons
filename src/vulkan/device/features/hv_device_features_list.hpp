#ifndef HVDEVICEFEATURESLISTHPP
#define HVDEVICEFEATURESLISTHPP

#define IHV_MODULE_DEVICE_FEATURE(u_feature_name, d_feature_name) \
    class HVfeature##u_feature_name : public IHVmodule { \
    public: \
        const char* get_module_name() const override { \
            return #d_feature_name; \
        } \
        IHVmodule* initialize() const override { \
            return new HVfeature##u_feature_name(); \
        } \
    };

IHV_MODULE_DEVICE_FEATURE(MultiDrawIndirect, multiDrawIndirect)
IHV_MODULE_DEVICE_FEATURE(DynamicRendering, dynamicRendering)

#endif // HVDEVICEFEATURESLISTHPP