#ifndef HVINSTANCELAYERLISTHPP
#define HVINSTANCELAYERLISTHPP

#define IHV_MODULE_INSTANCE_LAYER(layer_name_class, layer_name_vulkan) \
	class HVlayer##layer_name_class : public IHVmodule { \
	public: \
		const char* get_module_name() const override { \
			return "VK_LAYER_" #layer_name_vulkan; \
		} \
		IHVmodule* initialize() const override { \
			return new HVlayer##layer_name_class(); \
		} \
	};

IHV_MODULE_INSTANCE_LAYER(KhronosProfiles, KHRONOS_profiles)
IHV_MODULE_INSTANCE_LAYER(KhronosShaderObject, KHRONOS_shader_object)
IHV_MODULE_INSTANCE_LAYER(Synchronization2, KHRONOS_synchronization2)
IHV_MODULE_INSTANCE_LAYER(KhronosValidation, KHRONOS_validation)
IHV_MODULE_INSTANCE_LAYER(LunargApiDump, LUNARG_api_dump)
IHV_MODULE_INSTANCE_LAYER(LunargCrashDiagnostic, LUNARG_crash_diagnostic)
IHV_MODULE_INSTANCE_LAYER(LunargGFXreconstruct, LUNARG_gfxreconstruct)
IHV_MODULE_INSTANCE_LAYER(LunargMonitor, LUNARG_monitor)
IHV_MODULE_INSTANCE_LAYER(LunargScreenshot, LUNARG_screenshot)
IHV_MODULE_INSTANCE_LAYER(AMDswitchableGraphics, AMD_switchable_graphics)
IHV_MODULE_INSTANCE_LAYER(NVoptimus, NV_optimus)
IHV_MODULE_INSTANCE_LAYER(VALVEsteamFossilize, VALVE_steam_fossilize)
IHV_MODULE_INSTANCE_LAYER(VALVEsteamOverlay, VALVE_steam_overlay)

#endif // HVINSTANCELAYERLISTHPP