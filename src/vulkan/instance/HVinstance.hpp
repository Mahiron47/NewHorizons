#ifndef HVINSTANCEHPP
#define HVINSTANCEHPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>
#include <stdint.h>
#include <string>

#include "vulkan/instance/extensions/HVinstanceExtensions.hpp"
#include "vulkan/instance/layers/HVinstanceLayers.hpp"

class HVinstance {
// vulkan fields
 	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;

// layers and extensions structures
	const HVinstanceExtensions* _extensions;
	const HVinstanceLayers* _layers;

// vulkan version
    uint32_t _vulkan_version;

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
	HVinstance() : _instance(VK_NULL_HANDLE), 
				   _debug_messenger(VK_NULL_HANDLE),
				   _extensions(nullptr),
				   _layers(nullptr),
                   _vulkan_version(VK_API_VERSION_1_0) {}
    HVinstance(VkInstance instance,
               uint32_t vulkan_version) : _instance(instance),
                                          _debug_messenger(VK_NULL_HANDLE),
                                          _extensions(nullptr),
                                          _layers(nullptr),
                                          _vulkan_version(vulkan_version) {};
    HVinstance(VkInstance instance,
               HVinstanceExtensions* extensions,
               uint32_t vulkan_version) : _instance(instance),
                                          _debug_messenger(VK_NULL_HANDLE),
                                          _extensions(extensions),
                                          _layers(nullptr),
                                          _vulkan_version(vulkan_version) {};
    HVinstance(VkInstance instance,
               VkDebugUtilsMessengerEXT debug_messenger,
               HVinstanceExtensions* extensions,
               HVinstanceLayers* layers,
               uint32_t vulkan_version) : _instance(instance),
                                          _debug_messenger(debug_messenger),
                                          _extensions(extensions),
                                          _layers(layers),
                                          _vulkan_version(vulkan_version) {};

	static HVinstance* reinitialize(uint32_t vulkan_version = VK_API_VERSION_1_3);
	static HVinstance* reinitialize(HVinstanceExtensions&& extensions,
                                    uint32_t vulkan_version = VK_API_VERSION_1_3);
    static HVinstance* reinitialize(HVinstanceExtensions&& extensions,
                                    HVinstanceLayers&& layers,
                                    uint32_t vulkan_version = VK_API_VERSION_1_3);

public:
    static HVinstance* initialize(uint32_t vulkan_version = VK_API_VERSION_1_4);
    static HVinstance* initialize(HVinstanceExtensions&& extensions,
                                  uint32_t vulkan_version = VK_API_VERSION_1_4);
    static HVinstance* initialize(HVinstanceExtensions&& extensions,
                                  HVinstanceLayers&& layers,
                                  uint32_t vulkan_version = VK_API_VERSION_1_4);

    constexpr const HVinstanceExtensions* get_extensions() const noexcept { return _extensions; }
    constexpr uint32_t get_vulkan_version() const noexcept { return _vulkan_version; }

	constexpr operator VkInstance() const { return _instance; }

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
	HVinstance(HVinstance&& other) noexcept : _instance(other._instance), 
											  _debug_messenger(other._debug_messenger),
											  _extensions(other._extensions),
											  _layers(other._layers),
											  _vulkan_version(other._vulkan_version) {
		other._instance = VK_NULL_HANDLE;
		other._debug_messenger = VK_NULL_HANDLE;
		other._extensions = nullptr;
		other._layers = nullptr;
        other._vulkan_version = VK_API_VERSION_1_0;
	}
	HVinstance& operator=(HVinstance&& other) noexcept {
		if (this == &other) return *this;

     	this->_instance = other._instance;
     	this->_debug_messenger = other._debug_messenger;
		this->_extensions = other._extensions;
		this->_layers = other._layers;
        this->_vulkan_version = other._vulkan_version;

     	other._instance = VK_NULL_HANDLE;
     	other._debug_messenger = VK_NULL_HANDLE;
		other._extensions = nullptr;
		other._layers = nullptr;
        other._vulkan_version = VK_API_VERSION_1_0;

     	return *this;
	}
	HVinstance(const HVinstance&) = delete;
	HVinstance& operator=(const HVinstance&) = delete;
};

inline VkResult HVinstance::vkCreateDebugUtilsMessenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

inline void HVinstance::vkDestroyDebugUtilsMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) func(instance, debugMessenger, pAllocator);
}

inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";
	return VK_FALSE;
}

inline HVinstance* HVinstance::initialize(uint32_t vulkan_version) {
    VkInstance instance;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&create_info, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            return reinitialize();
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */

    return new HVinstance(instance, vulkan_version);
}

inline HVinstance* HVinstance::initialize(HVinstanceExtensions&& extensions,
                                          uint32_t vulkan_version) {
    VkInstance instance;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = extensions.get_size(),
        .ppEnabledExtensionNames = extensions.get_data()
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&create_info, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            return reinitialize(std::forward<HVinstanceExtensions>(extensions));
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */

    return new HVinstance(instance, new HVinstanceExtensions(std::move(extensions)), vulkan_version);
}

inline HVinstance* HVinstance::initialize(HVinstanceExtensions&& extensions,
                                          HVinstanceLayers&& layers,
                                          uint32_t vulkan_version) {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = layers.get_size(),
        .ppEnabledLayerNames = layers.get_data(),
        .enabledExtensionCount = extensions.get_size(),
        .ppEnabledExtensionNames = extensions.get_data()
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&create_info, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                std::forward<HVinstanceLayers>(layers));
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */
    
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr
    };

    /* VKDEBUGUTILS CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateDebugUtilsMessenger(instance,
                                                      &debug_create_info,
                                                      nullptr,
                                                      &debug_messenger);
        
        if (result != VK_SUCCESS) throw std::runtime_error("HVinstance:initialize : Failed to create debug messenger!");
    } /* VKDEBUGUTILS CREATION WITH SUCCESS CHECK */

    return new HVinstance(instance, debug_messenger,
                          new HVinstanceExtensions(std::move(extensions)),
                          new HVinstanceLayers(std::move(layers)), vulkan_version);
}

inline HVinstance* HVinstance::reinitialize(uint32_t vulkan_version) {
    VkInstance instance;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHengine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&create_info, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            switch (vulkan_version) {
            case VK_API_VERSION_1_3:
                return reinitialize(VK_API_VERSION_1_2);
            case VK_API_VERSION_1_2:
                return reinitialize(VK_API_VERSION_1_1);
            case VK_API_VERSION_1_1:
                return reinitialize(VK_API_VERSION_1_0);
            default:
                throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance due to incompatible driver!");
            }
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance.");
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */    

    return new HVinstance(instance, vulkan_version);
}

inline HVinstance* HVinstance::reinitialize(HVinstanceExtensions&& extensions,
                                            uint32_t vulkan_version) {
    VkInstance instance;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHengine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = extensions.get_size(),
        .ppEnabledExtensionNames = extensions.get_data()
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&create_info, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            switch (vulkan_version) {
            case VK_API_VERSION_1_3:
                return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                    VK_API_VERSION_1_2);
            case VK_API_VERSION_1_2:
                return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                    VK_API_VERSION_1_1);
            case VK_API_VERSION_1_1:
                return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                    VK_API_VERSION_1_0);
            default:
                throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance due to incompatible driver!");
            }
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance.");
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */    

    return new HVinstance(instance, new HVinstanceExtensions(std::move(extensions)), vulkan_version);
}

inline HVinstance* HVinstance::reinitialize(HVinstanceExtensions&& extensions,
                                            HVinstanceLayers&& layers,
                                            uint32_t vulkan_version) {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHengine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = layers.get_size(),
        .ppEnabledLayerNames = layers.get_data(),
        .enabledExtensionCount = extensions.get_size(),
        .ppEnabledExtensionNames = extensions.get_data()
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&create_info, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            switch (vulkan_version) {
            case VK_API_VERSION_1_3:
                return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                    std::forward<HVinstanceLayers>(layers),
                                    VK_API_VERSION_1_2);
            case VK_API_VERSION_1_2:
                return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                    std::forward<HVinstanceLayers>(layers),
                                    VK_API_VERSION_1_1);
            case VK_API_VERSION_1_1:
                return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                    std::forward<HVinstanceLayers>(layers),
                                    VK_API_VERSION_1_0);
            default:
                throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance due to incompatible driver!");
            }
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance.");
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */    

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr
    };

    /* VKDEBUGUTILS CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateDebugUtilsMessenger(instance,
                                                      &debug_create_info,
                                                      nullptr,
                                                      &debug_messenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create debug messenger!");
        }
    } /* VKDEBUGUTILS CREATION WITH SUCCESS CHECK */

    return new HVinstance(instance, debug_messenger,
                          new HVinstanceExtensions(std::move(extensions)),
                          new HVinstanceLayers(std::move(layers)), vulkan_version);
}

#endif // HVINSTANCEHPP