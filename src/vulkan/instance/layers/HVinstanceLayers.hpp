#ifndef HVINSTANCELAYERSHPP
#define HVINSTANCELAYERSHPP

#include "vulkan/interfaces/IHVmodules.hpp"
#include "memory/Hstack.hpp"

#include <vulkan/vulkan.hpp>

class HVlayerKhronosProfiles : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_KHRONOS_profiles";
	}
	IHVmodule* initialize() const override {
		return new HVlayerKhronosProfiles();
	}

};

class HVlayerKhronosShaderObject : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_KHRONOS_shader_object";
	}
	IHVmodule* initialize() const override {
		return new HVlayerKhronosShaderObject();
	}

};

class HVlayerSynchronization2 : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_KHRONOS_synchronization2";
	}
	IHVmodule* initialize() const override {
		return new HVlayerSynchronization2();
	}

};

class HVlayerKhronosValidation : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_KHRONOS_validation";
	}
	IHVmodule* initialize() const override {
		return new HVlayerKhronosValidation();
	}

};

class HVlayerLunargApiDump : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_LUNARG_api_dump";
	}
	IHVmodule* initialize() const override {
		return new HVlayerLunargApiDump();
	}

};

class HVlayerLunargCrashDiagnostic : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_LUNARG_crash_diagnostic";
	}
	IHVmodule* initialize() const override {
		return new HVlayerLunargCrashDiagnostic();
	}

};

class HVlayerLunargGFXreconstruct : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_LUNARG_gfxreconstruct";
	}
	IHVmodule* initialize() const override {
		return new HVlayerLunargGFXreconstruct();
	}

};

class HVlayerLunargMonitor : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_LUNARG_monitor";
	}
	IHVmodule* initialize() const override {
		return new HVlayerLunargMonitor();
	}

};

class HVlayerLunargScreenshot : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_LUNARG_screenshot";
	}
	IHVmodule* initialize() const override {
		return new HVlayerLunargScreenshot();
	}

};

class HVlayerAMDswitchableGraphics : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_AMD_switchable_graphics";
	}
	IHVmodule* initialize() const override {
		return new HVlayerAMDswitchableGraphics();
	}

};

class HVlayerNVoptimus : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_NV_optimus";
	}
	IHVmodule* initialize() const override {
		return new HVlayerNVoptimus();
	}

};

class HVlayerVALVEsteamFossilize : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_VALVE_steam_fossilize";
	}
	IHVmodule* initialize() const override {
		return new HVlayerVALVEsteamFossilize();
	}

};

class HVlayerVALVEsteamOverlay : public IHVmodule {
public:
	const char* get_module_name() const override {
		return "VK_LAYER_VALVE_steam_overlay";
	}
	IHVmodule* initialize() const override {
		return new HVlayerVALVEsteamOverlay();
	}

};

class HVinstanceLayers : public IHVmodules {
	void init_modules_list() override;

public:
	template<class... Args> requires (std::derived_from<std::remove_reference_t<Args>, IHVmodule> && ...)
    HVinstanceLayers(Args&&... args) : IHVmodules(std::forward<Args>(args)...) {
		this->initialize();
	}
};

inline void HVinstanceLayers::init_modules_list() {
	VkResult result;
	uint32_t layer_count;

	result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	if (result != VK_SUCCESS) throw std::runtime_error("HVinstanceLayers::init_modules_list : instance layers count do not acquired.");
	
	VkLayerProperties* layer_properties = new VkLayerProperties[layer_count];

	result = vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);
	
	if (result != VK_SUCCESS) throw std::runtime_error("HVinstanceLayers::init_modules_list : instance layers pointer do not acquired.");

    this->_modules_list.do_each([](IHVmodule*& val,
								 uint32_t layer_count,
								 VkLayerProperties* layer_properties) -> void {
		for (uint32_t i = 0; i < layer_count; i++) {
			if (strcmp(val->get_module_name(), layer_properties[i].layerName) == 0) {
				val->set_accesability(true);
				return;
			}
		}
		throw std::runtime_error("HVinstanceLayers::init_modules_list : instance layer " + 
			string(val->get_module_name()) + " is not supported.");  
    }, layer_count, layer_properties);

	delete[] layer_properties;
}

#endif /* HVINSTANCELAYERSHPP */