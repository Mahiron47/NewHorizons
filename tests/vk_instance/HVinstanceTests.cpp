#include <catch2/catch_test_macros.hpp>

#include "Horizon.hpp"

TEST_CASE("HVinstance::initialize", "[HVinstance]") {
    SECTION("HVinstance::initialize : simple check") {
        HVinstance* instance = HVinstance::initialize();

        REQUIRE(instance != nullptr);

        delete instance;
    }
}