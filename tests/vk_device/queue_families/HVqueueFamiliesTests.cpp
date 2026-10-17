#include <catch2/catch_all.hpp>

#include "Horizon.hpp"

TEST_CASE("HVqueueFamiliesController::get_queue_family ; IHVqueueFamilySetStrategy", "[HVqueueFamiliesController]") {
    Hwindow* window = Hwindow::initialize("HVdeviceTests window", 
                                          "HVdeviceTests window", 
                                          HwindowExtent { .width = 100, 
                                                          .height = 100 });
    HVinstance* instance = HVinstance::initialize(HVinstanceExtensions { HVextensionKHRsurface(), HVextensionKHRWIN32surface(), HVextensionEXTdebugUtils() },
                                                  HVinstanceLayers { HVlayerKhronosValidation() });
    HVsurface* surface = HVsurface::initialize(instance, window);
    HVphysicalDeviceController* phys_device_controller = HVphysicalDeviceController::initialize(instance);
    HVphysicalDevice* phys_device = phys_device_controller->pick_device(EHVsupportType::VK_VER_1_4 | EHVefficiencyType::BEST_PERFORMANCE);
    HVdevice* device = HVdevice::initialize(phys_device, instance, new HVgraphicSet1(phys_device, surface), 
                                            HVdeviceExtensions { phys_device, HVextensionKHRswapchain() });
    
    SECTION("HVqueueFamiliesController::get_queue_family ; IHVqueueFamilySetStrategy : hard test") {
        HVqueueFamilyController* queue_family = device->get_queue_family_controller();
        
        REQUIRE((queue_family->get_queue_family_count() == 3));
        REQUIRE((queue_family->get_queue_family(0)->get_queue_type() && VK_QUEUE_GRAPHICS_BIT));
        REQUIRE((queue_family->get_queue_family(1)->get_queue_type() && VK_QUEUE_TRANSFER_BIT));
        REQUIRE((queue_family->get_queue_family(2)->get_queue_type() && VK_QUEUE_COMPUTE_BIT));
    }
        
    delete device;
    delete phys_device_controller;
    delete surface;
    delete instance;
    delete window;
}