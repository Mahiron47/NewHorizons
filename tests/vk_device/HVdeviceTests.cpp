#include "catch2/catch_all.hpp"

#include "Horizon.hpp"

TEST_CASE("HVdevice::initialize : simple test", "[HVdevice]") {
    Hwindow* window = Hwindow::initialize("HVdeviceTests window", 
                                          "HVdeviceTests window", 
                                          HwindowExtent { .width = 100, 
                                                          .height = 100 });
    HVinstance* instance = HVinstance::initialize(HVinstanceExtensions { HVextensionKHRsurface(), HVextensionKHRWIN32surface(), HVextensionEXTdebugUtils() },
                                                  HVinstanceLayers { HVlayerKhronosValidation() });
    HVsurface* surface = HVsurface::initialize(instance, window);
    HVphysicalDeviceController* phys_device_controller = HVphysicalDeviceController::initialize(instance);
    HVphysicalDevice* phys_device = phys_device_controller->pick_device(EHVsupportType::VK_VER_1_0 | EHVefficiencyType::BEST_PERFORMANCE);
    HVdevice* device = HVdevice::initialize(phys_device, instance, new HVgraphicSet1(phys_device, surface), 
                                            HVdeviceExtensions { phys_device, HVextensionKHRswapchain() },
                                            HVdeviceFeatures { phys_device, HVfeatureMultiDrawIndirect() });

    vkDeviceWaitIdle(*device);

    delete device;
    delete phys_device_controller;
    delete surface;
    delete instance;
    delete window;

    REQUIRE(true);
}