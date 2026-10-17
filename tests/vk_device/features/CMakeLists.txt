cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_device_features HVdeviceFeaturesTests.cpp)

target_link_libraries(test_vk_device_features PRIVATE Horizons
                                                   Catch2::Catch2
                                                   Catch2::Catch2WithMain)

catch_discover_tests(test_vk_device_features TEST_PREFIX "vk_deviceZZvk_device_featuresZZ"
                                          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})