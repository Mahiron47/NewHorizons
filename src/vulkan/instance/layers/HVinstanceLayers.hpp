#ifndef HV_INSTANCE_LAYERS_HPP
#define HV_INSTANCE_LAYERS_HPP

#include "vulkan/interfaces/IHVinstanceModule.hpp"
#include "memory/Hstack.hpp"

#include <vulkan/vulkan.hpp>

struct IHVinstanceLayersPtr {
    IHVinstanceLayers* ptr;
    IHVinstanceLayersPtr() : ptr(nullptr) {}
    IHVinstanceLayersPtr(IHVinstanceLayers* ptr) : ptr(ptr) {}
    ~IHVinstanceLayersPtr() {
        delete ptr;
    }
    operator IHVinstanceLayers*() {
        return ptr;
    }

	IHVinstanceLayersPtr(const IHVinstanceLayersPtr& other) : ptr(other.ptr) {}
	IHVinstanceLayersPtr& operator=(const IHVinstanceLayersPtr& other) {
		this->ptr = other.ptr;
	}
	IHVinstanceLayersPtr(IHVinstanceLayersPtr&& other) noexcept : ptr(other.ptr) {
		other.ptr = nullptr;
	}
	IHVinstanceLayersPtr& operator=(IHVinstanceLayersPtr&& other) noexcept {
		this->ptr = other.ptr;
		other.ptr = nullptr;
	}
};

class HVlayerKhronosValidation : public IHVinstanceLayers {
	const char* vk_layer_khronos_validation_name = "VK_LAYER_KHRONOS_validation";
public:
	const char* get_layer_name() const override {
		return vk_layer_khronos_validation_name;
	}
	IHVinstanceLayersPtr&& initialize() const override {
		return IHVinstanceLayersPtr { new HVlayerKhronosValidation() };
	}

};

class HVlayerNVoptimus : public IHVinstanceLayers {
	const char* vk_layer_nv_optimus = "VK_LAYER_NV_optimus";
public:
	const char* get_layer_name() const override {
		return vk_layer_nv_optimus;
	}
	IHVinstanceLayersPtr&& initialize() const override {
		return IHVinstanceLayersPtr { new HVlayerNVoptimus() };
	}

};

class HVinstanceLayers {
	static const Hstack<uint32_t, IHVinstanceLayersPtr> full_layers_list;
	Hstack<uint32_t, IHVinstanceLayersPtr> layers_list;

	void init_layers();

public:
	template<typename... Args> requires (std::derived_from<Args, IHVinstanceLayers> && ...) 
	HVinstanceLayers(Args&&... layers);

	HVinstanceLayers(const HVinstanceLayers& other) {
		this->layers_list = other.layers_list;
	}
	HVinstanceLayers(HVinstanceLayers&& other) {
		this->layers_list = static_cast<Hstack<uint32_t, IHVinstanceLayersPtr>&&>(other.layers_list);

		other.layers_list = nullptr;
	}
	HVinstanceLayers& operator=(const HVinstanceLayers& other) {
		if (this == &other) return *this;
		
		this->layers_list = other.layers_list;

		return *this;
	}
	HVinstanceLayers& operator=(HVinstanceLayers&& other) {
		if (this == &other) return *this;
		
		this->layers_list = static_cast<Hstack<uint32_t, IHVinstanceLayersPtr>&&>(other.layers_list);

		other.layers_list = nullptr;

		return *this;
	}

};

inline const Hstack<uint32_t, IHVinstanceLayersPtr> HVinstanceLayers::full_layers_list = {
	new HVlayerKhronosValidation(), new HVlayerNVoptimus()
};

template<typename... Args> requires (std::derived_from<Args, IHVinstanceLayers> && ...)
inline HVinstanceLayers::HVinstanceLayers(Args&&... layers) {
	Hstack<uint32_t, IHVinstanceLayersPtr> stack = {
		(layers.initialize(), ...)
	};

	auto comparator = [&stack, this](uint32_t i) -> IHVinstanceLayersPtr {
		IHVinstanceLayersPtr value = nullptr;

		stack.do_each([](IHVinstanceLayersPtr& val, const Hstack<uint32_t, IHVinstanceLayersPtr>& list, uint32_t i) -> bool {
			return strcmp(val.ptr->get_layer_name(), list[i].ptr->get_layer_name()) == 0;
		}, &value, this->full_layers_list, i);

		return value;
	};

	for (uint32_t i = 0; i < sizeof...(layers); i++) {
		IHVinstanceLayersPtr obj = comparator(i);
		if (obj != nullptr) this->layers_list.push(obj);
	}

	this->init_layers();
}

inline void HVinstanceLayers::init_layers() {
	VkResult result;
	uint32_t layer_count;

	result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	if (result != VK_SUCCESS) throw std::runtime_error("HVinstanceLayers::init_layers : instance layers count do not acquired.");
	
	VkLayerProperties* layer_properties;

	result = vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);
	if (result != VK_SUCCESS) throw std::runtime_error("HVinstanceLayers::init_layers : instance layers pointer do not acquired.");

    this->layers_list.do_each([](IHVinstanceLayersPtr& val,
								 uint32_t layer_count,
								 VkLayerProperties* layer_properties) -> void {
		for (uint32_t i = 0; i < layer_count; i++) {
			if (strcmp(val.ptr->get_layer_name(), layer_properties[i].layerName) == 0) {
				val.ptr->set_accesability(true);
				return;
			}
		}
		throw std::runtime_error("HVinstanceLayers::init_layers : instance layer " + 
			string(val.ptr->get_layer_name()) + " is not supported.");  
    }, layer_count, layer_properties);
}

#endif /* HV_INSTANCE_LAYERS_HPP */