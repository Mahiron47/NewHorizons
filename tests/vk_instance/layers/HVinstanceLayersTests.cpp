#include <catch2/catch_test_macros.hpp>

#include "Horizon.hpp"

TEST_CASE("HVinstanceLayers::HVinstanceLayers(Args) : simple check", "[HVinstanceLayers]") {
    SECTION("Test 0") {
        IHVmodule* khronos_validation = HVlayerKhronosValidation().initialize();
        IHVmodule* nv_optimus = HVlayerNVoptimus().initialize();

        REQUIRE(khronos_validation != nullptr);
        REQUIRE(nv_optimus != nullptr);

        REQUIRE(khronos_validation->get_accesability() == false);
        REQUIRE(nv_optimus->get_accesability() == false);

        REQUIRE(khronos_validation->get_module_name() == std::string("VK_LAYER_KHRONOS_validation"));
        REQUIRE(nv_optimus->get_module_name() == std::string("VK_LAYER_NV_optimus"));

        delete khronos_validation;
        delete nv_optimus;
    }

    SECTION("Test 1") {
        Hstack<uint32_t, IHVmodule*> stack = {
            HVlayerKhronosValidation().initialize(), HVlayerNVoptimus().initialize()
        };

        REQUIRE(stack.size() == 2);

        REQUIRE(stack[0] != nullptr);
        REQUIRE(stack[1] != nullptr);

        REQUIRE(stack[0]->get_accesability() == false);
        REQUIRE(stack[1]->get_accesability() == false);

        REQUIRE(stack[0]->get_module_name() == std::string("VK_LAYER_KHRONOS_validation"));
        REQUIRE(stack[1]->get_module_name() == std::string("VK_LAYER_NV_optimus"));

        delete stack[0];
        delete stack[1];
    }
}

TEST_CASE("HVinstanceLayers::HVinstanceLayers(Args) : medium check", "[HVinstanceLayers]") {
    SECTION("Test 2") {
        HVinstanceLayers layers = HVinstanceLayers {
            HVlayerKhronosValidation()
        };

        REQUIRE(layers.get_size() == 1);
        REQUIRE(layers.get_data()[0] == std::string("VK_LAYER_KHRONOS_validation"));
    }

    SECTION("Test 3") {
        REQUIRE_THROWS_AS(HVinstanceLayers(
            HVlayerNVoptimus()
        ), std::runtime_error);
    }
}

TEST_CASE("HVinstanceLayers::HVinstanceLayers(Args) : hard check", "[HVinstanceLayers]") {
    SECTION("Test 4") {
        HVinstanceLayers layers = HVinstanceLayers {
            HVlayerKhronosValidation(), HVlayerKhronosShaderObject(), HVlayerSynchronization2()
        };

        REQUIRE(layers.get_size() == 3);
        REQUIRE(layers.get_data()[0] == std::string("VK_LAYER_KHRONOS_validation"));
        REQUIRE(layers.get_data()[1] == std::string("VK_LAYER_KHRONOS_shader_object"));
        REQUIRE(layers.get_data()[2] == std::string("VK_LAYER_KHRONOS_synchronization2"));
    }

    SECTION("Test 5") {
        REQUIRE_THROWS_AS(HVinstanceLayers(
            HVlayerKhronosValidation(), HVlayerNVoptimus(), 
            HVlayerKhronosShaderObject(), HVlayerSynchronization2()
        ), std::runtime_error);
    }
}