#include <catch2/catch_all.hpp>

#include "Horizon.hpp"

TEST_CASE("HVdeviceExtensions::init_modules_list ; IHVmodule", "[HVdeviceExtensions]") {
    Hwindow* window = Hwindow::initialize("HVdeviceTests window", 
                                          "HVdeviceTests window", 
                                          HwindowExtent { .width = 100, 
                                                          .height = 100 });
    HVinstance* instance = HVinstance::initialize(HVinstanceExtensions { HVextensionKHRsurface(), HVextensionKHRWIN32surface(), HVextensionEXTdebugUtils() },
                                                  HVinstanceLayers { HVlayerKhronosValidation() });
    HVsurface* surface = HVsurface::initialize(instance, window);
    HVphysicalDeviceController* phys_device_controller = HVphysicalDeviceController::initialize(instance);
    HVphysicalDevice* phys_device = phys_device_controller->pick_device(EHVsupportType::VK_VER_1_4 | EHVefficiencyType::BEST_PERFORMANCE);

    
    SECTION("HVdeviceExtensions::init_modules_list : simple check") {
        HVdeviceExtensions* extensions = new HVdeviceExtensions(phys_device, HVextensionKHRswapchain());

        REQUIRE((extensions->get_size() == 1));
        REQUIRE((strcmp(extensions->get_data()[0], "VK_KHR_swapchain") == 0));
        
        delete extensions;
    }

    SECTION("HVdeviceExtensions::init_modules_list : hard check") {
        HVdeviceExtensions* extensions = new HVdeviceExtensions(phys_device, HVextensionKHRswapchain(), 
                                                                HVextensionKHRmaintenance1(), HVextensionKHRmaintenance2());
        
        REQUIRE((extensions->get_size() == 3));
        REQUIRE((strcmp(extensions->get_data()[0], "VK_KHR_swapchain") == 0));
        REQUIRE((strcmp(extensions->get_data()[1], "VK_KHR_maintenance1") == 0));
        REQUIRE((strcmp(extensions->get_data()[2], "VK_KHR_maintenance2") == 0));

        delete extensions;
    }

    SECTION("HVdeviceExtensions::init_modules_list : unsupported extension throws std::runtime_error") {
        REQUIRE_THROWS_AS(new HVdeviceExtensions(phys_device, HVextensionKHRswapchain(), 
                                                 HVextensionKHRmaintenance1(), HVextensionKHRmaintenance2(), 
                                                 HVextensionEXTdebugUtils()), std::runtime_error);
    }

    delete phys_device_controller;
    delete surface;
    delete instance;
    delete window;
}