#include <catch2/catch_test_macros.hpp>

#include "Horizon.hpp"

TEST_CASE("HVinstanceExtensions::HVinstanceExtensions(Args) : simple check", "[HVinstanceExtensions]") {
    SECTION("Test 0") {
        IHVmodule* khr_surface = HVextensionKHRsurface().initialize();
        IHVmodule* ext_debug_utils = HVextensionEXTdebugUtils().initialize();

        REQUIRE(khr_surface != nullptr);
        REQUIRE(ext_debug_utils != nullptr);

        REQUIRE(khr_surface->get_accesability() == false);
        REQUIRE(ext_debug_utils->get_accesability() == false);

        REQUIRE(khr_surface->get_module_name() == std::string(VK_KHR_SURFACE_EXTENSION_NAME));
        REQUIRE(ext_debug_utils->get_module_name() == std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

        delete khr_surface;
        delete ext_debug_utils;
    }

    SECTION("Test 1") {
        Hstack<uint32_t, IHVmodule*> stack = {
            HVextensionKHRsurface().initialize(),
            HVextensionEXTdebugUtils().initialize()
        };

        REQUIRE(stack.size() == 2);

        REQUIRE(stack[0] != nullptr);
        REQUIRE(stack[1] != nullptr);

        REQUIRE(stack[0]->get_accesability() == false);
        REQUIRE(stack[1]->get_accesability() == false);

        REQUIRE(stack[0]->get_module_name() == std::string(VK_KHR_SURFACE_EXTENSION_NAME));
        REQUIRE(stack[1]->get_module_name() == std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

        delete stack[0];
        delete stack[1];
    }
}

TEST_CASE("HVinstanceExtensions::HVinstanceExtensions(Args) : medium check", "[HVinstanceExtensions]") {
    SECTION("Test 2") {
        HVinstanceExtensions extensions = HVinstanceExtensions {
            HVextensionKHRsurface()
        };

        REQUIRE(extensions.get_size() == 1);
        REQUIRE(extensions.get_data()[0] == std::string(VK_KHR_SURFACE_EXTENSION_NAME));
    }
}

TEST_CASE("HVinstanceExtensions::HVinstanceExtensions(Args) : hard check", "[HVinstanceExtensions]") {
    SECTION("Test 4") {
        HVinstanceExtensions extensions = HVinstanceExtensions {
            HVextensionKHRsurface(), HVextensionEXTdebugUtils()
        };

        REQUIRE(extensions.get_size() == 2);
        REQUIRE(extensions.get_data()[0] == std::string(VK_KHR_SURFACE_EXTENSION_NAME));
        REQUIRE(extensions.get_data()[1] == std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    }
}