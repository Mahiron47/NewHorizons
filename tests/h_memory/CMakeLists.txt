cmake_minimum_required(VERSION 3.10.0)

add_executable(test_h_memory HstackTests.cpp)

target_link_libraries(test_h_memory PRIVATE Horizons
											Catch2::Catch2
											Catch2::Catch2WithMain)

catch_discover_tests(test_h_memory TEST_PREFIX "h_memoryZZ" 
    							   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})