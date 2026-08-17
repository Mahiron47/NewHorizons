#ifndef HVINSTANCEHPP
#define HVINSTANCEHPP

#include "vulkan/instance/extensions/HVinstanceExtensions.hpp"
#include "vulkan/instance/layers/HVinstanceLayers.hpp"
#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <vulkan/vulkan.hpp>

class HVinstance {
// vulkan fields
 	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;

// layers and extensions structures
	const HVinstanceExtensions* _extensions;
	const HVinstanceLayers* _layers;

// debug callback function
	friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

// dynamic loading of debug utils functions
	static VkResult vkCreateDebugUtilsMessenger(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void vkDestroyDebugUtilsMessenger(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* pAllocator);

// internal main constructors
    HVinstance(VkInstance instance) : _instance(instance),
                                      _debug_messenger(VK_NULL_HANDLE),
                                      _extensions(nullptr),
                                      _layers(nullptr) {};
    HVinstance(VkInstance instance,
               HVinstanceExtensions&& extensions) : _instance(instance),
                                                    _debug_messenger(VK_NULL_HANDLE),
                                                    _extensions(new HVinstanceExtensions(std::forward<HVinstanceExtensions>(extensions))),
                                                    _layers(nullptr) {};
    HVinstance(VkInstance instance,
               VkDebugUtilsMessengerEXT debug_messenger,
               HVinstanceExtensions&& extensions,
               HVinstanceLayers&& layers) : _instance(instance),
                                          		 _debug_messenger(debug_messenger),
                                          		 _extensions(new HVinstanceExtensions(std::forward<HVinstanceExtensions>(extensions))),
                                          		 _layers(new HVinstanceLayers(std::forward<HVinstanceLayers>(layers))) {};

	static HVinstance* reinitialize(uint32_t vulkan_version = VK_API_VERSION_1_3);
	static HVinstance* reinitialize(HVinstanceExtensions&& extensions,
                                    uint32_t vulkan_version = VK_API_VERSION_1_3);
    static HVinstance* reinitialize(HVinstanceExtensions&& extensions,
                                    HVinstanceLayers&& layers,
                                    uint32_t vulkan_version = VK_API_VERSION_1_3);

public:
    static HVinstance* initialize();
    static HVinstance* initialize(HVinstanceExtensions&& extensions);
    static HVinstance* initialize(HVinstanceExtensions&& extensions,
                                  HVinstanceLayers&& layers);

// operators
	operator VkInstance() const { return _instance; }

// key constructors and assignments
	HVinstance(const HVinstance&) = delete;
	HVinstance& operator=(const HVinstance&) = delete;

	HVinstance(HVinstance&& other) noexcept : _instance(other._instance), 
											  _debug_messenger(other._debug_messenger),
											  _extensions(other._extensions),
											  _layers(other._layers) {
		other._instance = VK_NULL_HANDLE;
		other._debug_messenger = VK_NULL_HANDLE;
		other._extensions = nullptr;
		other._layers = nullptr;
	}

	HVinstance& operator=(HVinstance&& other) noexcept {
		if (this == &other) return *this;

     	this->_instance = other._instance;
     	this->_debug_messenger = other._debug_messenger;
		this->_extensions = other._extensions;
		this->_layers = other._layers;

     	other._instance = VK_NULL_HANDLE;
     	other._debug_messenger = VK_NULL_HANDLE;
		other._extensions = nullptr;
		other._layers = nullptr;

     	return *this;
	}

	HVinstance() : _instance(VK_NULL_HANDLE), 
				   _debug_messenger(VK_NULL_HANDLE),
				   _extensions(nullptr),
				   _layers(nullptr) {}

   ~HVinstance() {
    	if (_debug_messenger != VK_NULL_HANDLE) {
    	    vkDestroyDebugUtilsMessenger(_instance, _debug_messenger, nullptr);
    	} if (_instance != VK_NULL_HANDLE) {
    	    vkDestroyInstance(_instance, nullptr);
    	} if (_extensions != nullptr) {
			delete _extensions;
		} if (_layers != nullptr) {
			delete _layers;
		}
	}
};

#endif // HVINSTANCEHPP