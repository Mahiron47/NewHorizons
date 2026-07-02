#include "HVinstanceLayers.hpp"

HVinstanceLayers::HVinstanceLayers(std::initializer_list<const char*> layers) {
	
    auto comparator = [layers](const char* layer) -> bool {
		std::for_each(layers.begin(), layers.end(), [layer](const char* lyr) {
			if (strcmp(layer, lyr) == 0) {
				return true;
			}
		});
	};

	this->layers_list = {
		.vk_layer_khronos_validation = comparator(this->layers_list.vk_layer_khronos_validation_name)
	};

    this->init_layers();
}

void HVinstanceLayers::init_layers() {
    /* VALIDATION LAYERS AVAILABILITY CHECK */ {
        VkResult result;

        uint32_t available_layers_count;
        result = vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstanceLayers::init_layers : Failed to enumerate instance layer properties!");
        }

        std::vector<VkLayerProperties> available_layers(available_layers_count);
        result = vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers.data());

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstanceLayers::init_layers : Failed to enumerate instance layer properties!");
        }

		auto layers = available_layers; 

		auto comparator = [layers](const char* layer) -> bool {
			std::for_each(layers.begin(), layers.end(), [layer](const char* lyr) {
				if (strcmp(layer, lyr) == 0) {
					return true;
				}
			});
		};

		if (this->layers_list.vk_layer_khronos_validation != comparator(this->layers_list.vk_layer_khronos_validation_name)) {
			std::cout << "Layer VK_LAYER_KHRONOS_validation is not supported." << std::endl;
		}
         
    } /* VALIDATION LAYERS AVAILABILITY CHECK */
}