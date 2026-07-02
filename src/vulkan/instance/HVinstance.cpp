#include "HVinstance.hpp"
#include <stdexcept>
#include <iostream>

Hstack<uint32_t, HVinstance> HVinstance::instances;

static VkResult vkCreateDebugUtilsMessenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void vkDestroyDebugUtilsMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) func(instance, debugMessenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

HVinstance&& HVinstance::reinitialize(bool enableValidationLayers, uint32_t vulkan_version) {
    VkInstance instance; VkDebugUtilsMessengerEXT debugMessenger; 

    VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };
    
    const char* extensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

    uint32_t extensions_count = sizeof(extensions) / sizeof(extensions[0]);

    /* EXTENSIONS AVAILABILITY CHECK */ {
        VkResult result;

        uint32_t available_extensions_count;
        result = vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to enumerate instance extension properties!");
        }

        VkExtensionProperties* available_extensions = new VkExtensionProperties[available_extensions_count];
        result = vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, available_extensions);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to enumerate instance extension properties!");
        }

        for (uint32_t j = 0; j < extensions_count; j++) {
            for (uint32_t i = 0; i < available_extensions_count; i++) {
                if (strcmp(extensions[j], available_extensions[i].extensionName) == 0) {
                    std::cout << "Extension " << extensions[j] << " is supported." << std::endl;
                    break;
                } if (i == available_extensions_count - 1) {
                    std::cout << "HVinstance:initialize : Extension requested, but not available!" << std::endl;
                }
            }
        }

        delete[] available_extensions;
    } /* EXTENSIONS AVAILABILITY CHECK */

    const char* validation_layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	uint32_t validation_layers_count = sizeof(validation_layers) / sizeof(validation_layers[0]);

	/* VALIDATION LAYERS AVAILABILITY CHECK */ {
        VkResult result;

		uint32_t available_validation_layers_count;
		result = vkEnumerateInstanceLayerProperties(&available_validation_layers_count, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to enumerate instance layer properties!");
        }

		VkLayerProperties* available_validation_layers = new VkLayerProperties[available_validation_layers_count];
        result = vkEnumerateInstanceLayerProperties(&available_validation_layers_count, available_validation_layers);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to enumerate instance layer properties!");
        }

        for (uint32_t j = 0; j < validation_layers_count; j++) {
            for (uint32_t i = 0; i < available_validation_layers_count; i++) {
                if (strcmp(validation_layers[j], available_validation_layers[i].layerName) == 0) {
                    std::cout << "Validation layer " << validation_layers[j] << " is supported." << std::endl;
                    break;
                } if (i == available_validation_layers_count - 1) {
                    throw std::runtime_error("HVinstance:initialize : Validation layer requested, but not available!");
                }
            }
        }
 
        delete[] available_validation_layers;
	} /* VALIDATION LAYERS AVAILABILITY CHECK */

    VkInstanceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enableValidationLayers ? validation_layers_count : 0,
        .ppEnabledLayerNames = enableValidationLayers ? validation_layers : nullptr,
        .enabledExtensionCount = extensions_count,
        .ppEnabledExtensionNames = extensions
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&createInfo, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            switch (vulkan_version) {
                case VK_API_VERSION_1_3:
                    return reinitialize(enableValidationLayers, VK_API_VERSION_1_2);
                case VK_API_VERSION_1_2:
                    return reinitialize(enableValidationLayers, VK_API_VERSION_1_1);
                case VK_API_VERSION_1_1:
                    return reinitialize(enableValidationLayers, VK_API_VERSION_1_0);
                default:
                    throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance due to incompatible driver!");
            }
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create Vulkan instance.");
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */

    if (!enableValidationLayers) {
        return HVinstance(instance, debugMessenger);
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
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
                                                      &debugCreateInfo,
                                                      nullptr,
                                                      &debugMessenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create debug messenger!");
        }
    } /* VKDEBUGUTILS CREATION WITH SUCCESS CHECK */

    return HVinstance(instance, debugMessenger);
}

HVinstance& HVinstance::initialize(HVinstanceExtensions extensions, bool enableValidationLayers) {
    VkInstance instance; VkDebugUtilsMessengerEXT debugMessenger; 

    VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_4 
    };

    const char* validation_layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	uint32_t validation_layers_count = sizeof(validation_layers) / sizeof(validation_layers[0]);

	

    VkInstanceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enableValidationLayers ? validation_layers_count : 0,
        .ppEnabledLayerNames = enableValidationLayers ? validation_layers : nullptr,
        .enabledExtensionCount = extensions_count,
        .ppEnabledExtensionNames = extensions
    };

    /* INSTANCE CREATION WITH SUCCESS CHECK */ {
        VkResult result = vkCreateInstance(&createInfo, 
                                           nullptr, 
                                           &instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            vkDestroyInstance(instance, nullptr);
            instance = reinitialize(VK_API_VERSION_1_3, enableValidationLayers);
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */

    if (!enableValidationLayers) {
        HVinstance::instances.push(HVinstance(instance, VK_NULL_HANDLE));
        return HVinstance::instances.last();
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
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
                                                      &debugCreateInfo,
                                                      nullptr,
                                                      &debugMessenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("HVinstance:initialize : Failed to create debug messenger!");
        }
    } /* VKDEBUGUTILS CREATION WITH SUCCESS CHECK */

    HVinstance::instances.push(HVinstance(instance, debugMessenger));
    return HVinstance::instances.last();
}

HVinstance& HVinstance::operator=(HVinstance&& other) noexcept {
    if (this != &other) {
        if (debugMessenger != VK_NULL_HANDLE) {
            vkDestroyDebugUtilsMessenger(instance, debugMessenger, nullptr);
        } if (instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
        }

        instance = other.instance;
        debugMessenger = other.debugMessenger;

        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
    }
    return *this;
}

HVinstance::~HVinstance() {
    if (debugMessenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessenger(instance, debugMessenger, nullptr);
    } if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
    }
}

