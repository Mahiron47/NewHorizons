cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_physdevice HVphysicalDeviceTests.cpp)

target_link_libraries(test_vk_physdevice PRIVATE Horizons
										 		 Catch2::Catch2
										 		 Catch2::Catch2WithMain)

catch_discover_tests(test_vk_physdevice TEST_PREFIX "vk_physdeviceZZ" 
										WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(vk_support_types)