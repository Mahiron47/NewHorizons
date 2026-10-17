#ifndef HVINSTANCELAYERSHPP
#define HVINSTANCELAYERSHPP

#include "vulkan/instance/layers/hv_instance_layer_list.hpp"

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
								 std::string(val->get_module_name()) + " is not supported.");  
    }, layer_count, layer_properties);

	delete[] layer_properties;
}

#endif /* HVINSTANCELAYERSHPP */