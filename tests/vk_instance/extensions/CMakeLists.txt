cmake_minimum_required(VERSION 3.10.0)

add_executable(test_vk_instance_extensions HVinstanceExtensionsTests.cpp)

target_link_libraries(test_vk_instance_extensions PRIVATE Horizons
													   	  Catch2::Catch2
													   	  Catch2::Catch2WithMain)

catch_discover_tests(test_vk_instance_extensions TEST_PREFIX "vk_instanceZZvk_extensionsZZ" 
    								  			 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})