#ifndef HV_INSTANCE_LAYERS_HPP
#define HV_INSTANCE_LAYERS_HPP

#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <vulkan/vulkan.hpp>
#include <initializer_list>
#include <vector>

struct sHVinstanceLayers {
	// 1 group
	bool vk_layer_khronos_validation = false;
	const char* vk_layer_khronos_validation_name = "VK_LAYER_KHRONOS_validation";

};

class HVinstanceLayers {
	sHVinstanceLayers layers_list;


	void init_layers();
	
public:
	HVinstanceLayers(std::initializer_list<const char*> layers);

	HVinstanceLayers(const HVinstanceLayers&);



};

#endif /* HV_INSTANCE_LAYERS_HPP */