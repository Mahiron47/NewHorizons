cmake_minimum_required(VERSION 3.10.0)

add_executable(test_h_instances HVinstancesTests.cpp)

target_link_libraries(test_h_instances PRIVATE Horizons
                                         	   Catch2::Catch2
                                         	   Catch2::Catch2WithMain)

catch_discover_tests(test_h_instances TEST_PREFIX "h_instancesZZ"
                                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})