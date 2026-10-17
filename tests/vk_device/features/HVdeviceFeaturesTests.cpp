#include <catch2/catch_all.hpp>

#include "Horizon.hpp"

TEST_CASE("HVdeviceFeatures::init_modules_list ; IHVmodule", "[HVdeviceExtensions]") {
    Hwindow* window = Hwindow::initialize("HVdeviceTests window", 
                                          "HVdeviceTests window", 
                                          HwindowExtent { .width = 100, 
                                                          .height = 100 });
    HVinstance* instance = HVinstance::initialize(HVinstanceExtensions { HVextensionKHRsurface(), HVextensionKHRWIN32surface(), HVextensionEXTdebugUtils() },
                                                  HVinstanceLayers { HVlayerKhronosValidation() });
    HVsurface* surface = HVsurface::initialize(instance, window);
    HVphysicalDeviceController* phys_device_controller = HVphysicalDeviceController::initialize(instance);
    HVphysicalDevice* phys_device = phys_device_controller->pick_device(EHVsupportType::VK_VER_1_4 | EHVefficiencyType::BEST_PERFORMANCE);

    SECTION("HVdeviceFeatures::init_modules_list : simple check") {
        HVdeviceFeatures* features = new HVdeviceFeatures(phys_device, HVfeatureMultiDrawIndirect());

        REQUIRE((features->get_size() == 1));
        REQUIRE(features->get_features().multiDrawIndirect);
    
        delete features;
    }

    SECTION("HVdeviceFeatures::init_modules_list : unsupported extension throws std::runtime_error") {
        REQUIRE_THROWS_AS(new HVdeviceFeatures(phys_device, HVfeatureMultiDrawIndirect(), 
                                               HVextensionEXTdebugUtils()), std::runtime_error);
    }

    delete phys_device_controller;
    delete surface;
    delete instance;
    delete window;
}