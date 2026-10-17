#include <catch2/catch_all.hpp>

#include "Horizon.hpp"

TEST_CASE("Horizon : init vulkan.", "[Horizon]") {
    Hwindow* window = Hwindow::initialize("HVinstancesTests window", 
                                          "HVinstancesTests window", 
                                          HwindowExtent { .width = 100, 
                                                          .height = 100 });
    HVinstance* instance = HVinstance::initialize(HVinstanceExtensions { HVextensionKHRsurface(), HVextensionKHRWIN32surface(), HVextensionEXTdebugUtils() },
                                                  HVinstanceLayers { HVlayerKhronosValidation() });
    HVsurface* surface = HVsurface::initialize(instance, window);
    HVphysicalDeviceController* phys_device_controller = HVphysicalDeviceController::initialize(instance);
    HVphysicalDevice* phys_device = phys_device_controller->pick_device(EHVsupportType::VK_VER_1_0 | EHVefficiencyType::BEST_PERFORMANCE);
    HVdevice* device = HVdevice::initialize(phys_device, new HVgraphicSet1(phys_device, surface), 
                                            HVdeviceExtensions { phys_device, HVextensionKHRSwapchain() },
                                            HVdeviceFeatures { phys_device, HVfeatureMultiDrawIndirect() });
    HVswapchain* swapchain = HVswapchain::initialize(device, surface, window, new HVregularProgram());
    
    delete swapchain;
    delete device;
    delete phys_device_controller;
    delete surface;
    delete instance;
    delete window;
}