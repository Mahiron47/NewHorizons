#ifndef HV_INSTANCE_HPP
#define HV_INSTANCE_HPP

#include "vulkan/instance/extensions/HVinstanceExtensions.hpp"
#include "vulkan/instance/layers/HVinstanceLayers.hpp"
#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <vulkan/vulkan.hpp>

class HVinstance {
	static Hstack<uint32_t, HVinstance> instances;

// vulkan fields
 	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

// debug callback function
	friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

// internal main constructor
	HVinstance(VkInstance instance, 
			   VkDebugUtilsMessengerEXT debugMessenger) : instance(instance), 
			   											  debugMessenger(debugMessenger) {};

	static HVinstance&& reinitialize(bool enableValidationLayers, uint32_t vulkan_version = VK_API_VERSION_1_3);

public:
	static HVinstance& initialize(bool enableValidationLayers = false);

// operators
	static VkInstance operator[](uint32_t index) { return instances[index]; }
	operator VkInstance() const { return instance; }

// key constructors and operators
	HVinstance(HVinstance&& other) noexcept : instance(other.instance), 
											  debugMessenger(other.debugMessenger) {
		other.instance = VK_NULL_HANDLE;
		other.debugMessenger = VK_NULL_HANDLE;
	}

	HVinstance& operator=(HVinstance&& other) noexcept;

	HVinstance() : instance(VK_NULL_HANDLE), 
				   debugMessenger(VK_NULL_HANDLE) {}

   ~HVinstance();

	HVinstance(const HVinstance&) = delete;
	HVinstance& operator=(const HVinstance&) = delete;
};

#endif // HV_INSTANCE_HPP