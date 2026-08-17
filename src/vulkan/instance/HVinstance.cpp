#include "HVinstance.hpp"
#include <stdexcept>
#include <iostream>

VkResult HVinstance::vkCreateDebugUtilsMessenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void HVinstance::vkDestroyDebugUtilsMessenger(
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

HVinstance* HVinstance::reinitialize(uint32_t vulkan_version) {
    VkInstance instance;

    VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHengine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info {
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

    return new HVinstance(instance);
}

HVinstance* HVinstance::reinitialize(HVinstanceExtensions&& extensions,
                                     uint32_t vulkan_version) {
    VkInstance instance;

    VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHengine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info {
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

    return new HVinstance(instance, std::forward<HVinstanceExtensions>(extensions));
}

HVinstance* HVinstance::reinitialize(HVinstanceExtensions&& extensions,
                                     HVinstanceLayers&& layers,
                                     uint32_t vulkan_version) {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHengine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vulkan_version
    };

    VkInstanceCreateInfo create_info {
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

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info {
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
                          std::forward<HVinstanceExtensions>(extensions),
                          std::forward<HVinstanceLayers>(layers));
}

HVinstance* HVinstance::initialize() {
    VkInstance instance;

    VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_4 
    };

    VkInstanceCreateInfo create_info {
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
            return reinitialize(VK_API_VERSION_1_3);
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */

    return new HVinstance(instance);
}

HVinstance* HVinstance::initialize(HVinstanceExtensions&& extensions) {
    VkInstance instance;

    VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_4 
    };

    VkInstanceCreateInfo create_info {
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
            return reinitialize(std::forward<HVinstanceExtensions>(extensions),
                                VK_API_VERSION_1_3);
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */

    return new HVinstance(instance, std::forward<HVinstanceExtensions>(extensions));
}

HVinstance* HVinstance::initialize(HVinstanceExtensions&& extensions,
                                   HVinstanceLayers&& layers) {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "New Horizons",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "NHEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_4 
    };

    VkInstanceCreateInfo create_info {
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
                                std::forward<HVinstanceLayers>(layers),
                                VK_API_VERSION_1_3);
        } else if (result != VK_SUCCESS) {
            std::cout << "HVinstance:initialize : Failed to create Vulkan instance." << std::endl;
        }
    } /* INSTANCE CREATION WITH SUCCESS CHECK */
    
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info {
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
                          std::forward<HVinstanceExtensions>(extensions),
                          std::forward<HVinstanceLayers>(layers));
}
