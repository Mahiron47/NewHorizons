cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_support_types HphysicalDeviceTypeTests.cpp)

target_link_libraries(test_vk_support_types PRIVATE Horizons
										 		    Catch2::Catch2
										 		    Catch2::Catch2WithMain)

catch_discover_tests(test_vk_support_types TEST_PREFIX "vk_physdeviceZZvk_support_typesZZ" 
										   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})