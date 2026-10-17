#ifndef HVDEVICEFEATURESHPP
#define HVDEVICEFEATURESHPP

#include "vulkan/device/features/hv_device_features_list.hpp"

struct HVdeviceFeaturesPackage {
    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceVulkan11Features features11;
    VkPhysicalDeviceVulkan12Features features12;
    VkPhysicalDeviceVulkan13Features features13;
    VkPhysicalDeviceVulkan14Features features14;
};

class HVdeviceFeatures : public IHVmodules {
    HVphysicalDevice* _phys_device;
    
    void init_modules_list() override;

public: 
    template<class... Args> requires (std::derived_from<std::remove_reference_t<Args>, IHVmodule> && ...)
	HVdeviceFeatures(HVphysicalDevice* phys_device, Args&&... args) : IHVmodules(std::forward<Args>(args)... ),
                                                                      _phys_device(phys_device) {
		this->initialize();
	}

    VkPhysicalDeviceFeatures get_features() {        
        VkPhysicalDeviceFeatures features;

        this->_modules_list.do_each([](IHVmodule*& feature, VkPhysicalDeviceFeatures* features) -> void {
            // ———————————————————————————————————————————————————————————————————————————————————————————————————————————————————
            #define ADD_FEATURE(feature_name) \
                if (strcmp(feature->get_module_name(), #feature_name) == 0) { \
                    features->feature_name = feature->get_accesability() ? VK_TRUE : VK_FALSE; \
                    return; \
                }
            // ———————————————————————————————————————————————————————————————————————————————————————————————————————————————————

            ADD_FEATURE(multiDrawIndirect)
        }, &features);

        return features;
    }

    HVdeviceFeaturesPackage get_vulkan_features(HVinstance* instance) {
        uint32_t vulkan_version = instance->get_vulkan_version();
        
        if (vulkan_version <= VK_API_VERSION_1_0) throw std::runtime_error("HVdeviceFeatures::get_vulkan_features : Vulkan version 1.0 is not supported.");

        VkPhysicalDeviceFeatures2 features2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = nullptr,
            .features = this->get_features()
        };

        VkPhysicalDeviceVulkan11Features features11 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
            .pNext = nullptr
        };
        VkPhysicalDeviceVulkan12Features features12 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = nullptr
        };
        VkPhysicalDeviceVulkan13Features features13 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = nullptr
        };
        VkPhysicalDeviceVulkan14Features features14 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
            .pNext = nullptr
        };

        this->_modules_list.do_each([](IHVmodule*& feature, VkPhysicalDeviceVulkan11Features* features11,
                                                            VkPhysicalDeviceVulkan12Features* features12,
                                                            VkPhysicalDeviceVulkan13Features* features13,
                                                            VkPhysicalDeviceVulkan14Features* features14) -> void {
            // ———————————————————————————————————————————————————————————————————————————————————————————————————————————————————
            #define ADD_FEATURE_PROTOTYPE(features_name, feature_name) \
                if (strcmp(feature->get_module_name(), #feature_name) == 0) { \
                    features_name->feature_name = feature->get_accesability() ? VK_TRUE : VK_FALSE; \
                    return; \
                }
            #define ADD_FEATURE11(feature_name) ADD_FEATURE_PROTOTYPE(features11, feature_name)
            #define ADD_FEATURE12(feature_name) ADD_FEATURE_PROTOTYPE(features12, feature_name)
            #define ADD_FEATURE13(feature_name) ADD_FEATURE_PROTOTYPE(features13, feature_name)
            #define ADD_FEATURE14(feature_name) ADD_FEATURE_PROTOTYPE(features14, feature_name)
            // ———————————————————————————————————————————————————————————————————————————————————————————————————————————————————

            ADD_FEATURE13(dynamicRendering);
        }, &features11, &features12, &features13, &features14);   

        HVdeviceFeaturesPackage package = {
            .features2 = features2,
            .features11 = features11,
            .features12 = features12,
            .features13 = features13,
            .features14 = features14
        };
    
        if (vulkan_version >= VK_API_VERSION_1_4) package.features2.pNext = &package.features14;
        if (vulkan_version >= VK_API_VERSION_1_3) package.features12.pNext = &package.features13;
        if (vulkan_version >= VK_API_VERSION_1_2) package.features11.pNext = &package.features12;  
        package.features2.pNext = package.features11.pNext;

        return package;
    }

    const char** get_data() const override {
        throw std::runtime_error("HVdeviceFeatures::get_data : this method malfunction in this derived class.");
    }

};

inline void HVdeviceFeatures::init_modules_list() {
    VkPhysicalDeviceFeatures features;

    vkGetPhysicalDeviceFeatures(*_phys_device, &features);    

    this->_modules_list.do_each([](IHVmodule*& feature, const VkPhysicalDeviceFeatures& features) -> void {
        // ———————————————————————————————————————————————————————————————————————————————————————————————————————————————————
        #define CHECK_FEATURE1(feature_name) \
            if (strcmp(feature->get_module_name(), #feature_name) == 0) { \
                if (features.feature_name == 0) throw std::runtime_error("HVdeviceFeatures::init_modules_list : feature " + \
                                                                         std::string(feature->get_module_name()) + \
                                                                         " requested but unavailable."); \
                feature->set_accesability(true); \
                return; \
            }
        #define CHECK_FEATURE2(feature_name) \
            CHECK_FEATURE1(feature_name) 
        #define CHECK_FEATURE3(feature_name) \
            CHECK_FEATURE1(feature_name)
        #define CHECK_FEATURE4(feature_name) \
            CHECK_FEATURE1(feature_name)
        // ———————————————————————————————————————————————————————————————————————————————————————————————————————————————————
        CHECK_FEATURE1(multiDrawIndirect)
        
        throw std::runtime_error("HVdeviceFeatures::init_modules_list : feature " + 
                                 std::string(feature->get_module_name()) + 
                                 " requested but unsupported.");
    }, features);
}

#endif // HVDEVICEFEATURESHPP