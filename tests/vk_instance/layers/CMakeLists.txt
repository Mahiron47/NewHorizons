cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_instance_layers HVinstanceLayersTests.cpp)

target_link_libraries(test_vk_instance_layers PRIVATE Horizons
											   		  Catch2::Catch2
											   		  Catch2::Catch2WithMain)

catch_discover_tests(test_vk_instance_layers TEST_PREFIX "vk_instanceZZvk_layersZZ" 
    								  		 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})