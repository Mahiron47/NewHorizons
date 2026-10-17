#ifndef HVINSTANCEEXTENSIONSHPP
#define HVINSTANCEEXTENSIONSHPP

#include "vulkan/instance/extensions/hv_instance_extension_list.hpp"

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
                                 std::string(val->get_module_name()) + " is not supported.");
    }, extensions_count, extension_properties);

    delete[] extension_properties;
}

#endif /* HVINSTANCEEXTENSIONSHPP */