#include <catch2/catch_test_macros.hpp>

#include "vulkan/instance/layers/HVinstanceLayers.hpp"

TEST_CASE("HVinstanceLayers::HVinstanceLayers(Args)", "[HVinstanceLayers]") {
    SECTION("Test 1") {
        HVinstanceLayers layers = {
            HVlayerKhronosValidation(), HVlayerNVoptimus()
        };

        REQUIRE(1 == 1);
    }
}