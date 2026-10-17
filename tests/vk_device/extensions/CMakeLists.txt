cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_device_extensions HVdeviceExtensionsTests.cpp)

target_link_libraries(test_vk_device_extensions PRIVATE Horizons
                                                        Catch2::Catch2
                                                        Catch2::Catch2WithMain)

catch_discover_tests(test_vk_device_extensions TEST_PREFIX "vk_deviceZZvk_device_extensionsZZ"
                                               WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})