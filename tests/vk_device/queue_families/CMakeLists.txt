cmake_minimum_required(VERSION 3.10.0)

add_executable(test_queue_families HVqueueFamiliesTests.cpp)

target_link_libraries(test_queue_families PRIVATE Horizons
                                                  Catch2::Catch2
                                                  Catch2::Catch2WithMain)

catch_discover_tests(test_queue_families TEST_PREFIX "vk_deviceZZqueue_familiesZZ"
                                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})