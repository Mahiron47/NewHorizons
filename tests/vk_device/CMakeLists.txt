cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_device HVdeviceTests.cpp)

target_link_libraries(test_vk_device PRIVATE Horizons
                                             Catch2::Catch2
                                             Catch2::Catch2WithMain)

catch_discover_tests(test_vk_device TEST_PREFIX "vk_deviceZZ"
                                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(queue_families)
add_subdirectory(extensions)
add_subdirectory(features)