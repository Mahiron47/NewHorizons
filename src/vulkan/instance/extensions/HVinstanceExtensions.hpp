#ifndef HVINSTANCEEXTENSIONSHPP
#define HVINSTANCEEXTENSIONSHPP

#include "vulkan/interfaces/IHVmodules.hpp"
#include "memory/Hstack.hpp"

#include <vulkan/vulkan.hpp>

class HVextensionKHRsurface : public IHVmodule {
public:
    const char* get_module_name() const override {
        return VK_KHR_SURFACE_EXTENSION_NAME;
    }
    IHVmodule* initialize() const override {
        return new HVextensionKHRsurface();
    }

};

#ifdef VK_USE_PLATFORM_WIN32_KHR

class HVextensionKHRWIN32surface : public IHVmodule {
public:
    const char* get_module_name() const override {
        return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    }
    IHVmodule* initialize() const override {
        return new HVextensionKHRWIN32surface();
    }

};

#endif

#ifdef VK_USE_PLATFORM_XCB_KHR

class HVextensionKHRXCBsurface : public IHVmodule {
public:
    const char* get_module_name() const override {
		return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
    }
    IHVmodule* initialize() const override {
		return new HVextensionKHRXCBsurface();
    }

};

#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

class HVextensionKHRwaylandSurface : public IHVmodule {
public:
	const char* get_module_name() const override {
		return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
	}
    IHVmodule* initialize() const override {
        return new HVextensionKHRwaylandSurface();
    }

};

#endif

class HVextensionEXTdebugUtils : public IHVmodule {
public:
	const char* get_module_name() const override {
		return VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	}
    IHVmodule* initialize() const override {
        return new HVextensionEXTdebugUtils();
    }

};

class HVinstanceExtensions : public IHVmodules {
	void init_modules_list() override;
	
public:
    template<class... Args> requires (std::derived_from<std::remove_reference_t<Args>, IHVmodule> && ...)
    HVinstanceExtensions(Args&&... args) : IHVmodules(std::forward<Args>(args)...) {
        this->initialize();
    }

};

inline void HVinstanceExtensions::init_modules_list() {
    VkResult result;

    uint32_t extensions_count;
    
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

    if (result != VK_SUCCESS) throw std::runtime_error("HVinstanceExtensions::init_modules_list : Failed to enumerate instance extension properties!");
    

    VkExtensionProperties* extension_properties = new VkExtensionProperties[extensions_count];

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, extension_properties);

    if (result != VK_SUCCESS) throw std::runtime_error("HVinstanceExtensions::init_modules_list : Failed to enumerate instance extension properties!");
    

    this->_modules_list.do_each([](IHVmodule*& val,
                                     uint32_t extension_count,
                                     VkExtensionProperties* extension_properties) -> void {
        for (uint32_t i = 0; i < extension_count; i++) {
            if (strcmp(val->get_module_name(), extension_properties[i].extensionName) == 0) {
                val->set_accesability(true);
                return;
            }
        }
        throw std::runtime_error("HVinstanceExtensions::init_modules_list : instance extension " +
                                 string(val->get_module_name()) + " is not supported.");
    }, extensions_count, extension_properties);

    delete[] extension_properties;
}

#endif /* HVINSTANCEEXTENSIONSHPP */