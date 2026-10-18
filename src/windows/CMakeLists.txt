cmake_minimum_required(VERSION 3.16)

add_library(windows STATIC windows.cpp)

target_include_directories(windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    target_link_libraries(windows PUBLIC Vulkan::Vulkan)
endif()

if(WIN32)
    target_link_libraries(windows PRIVATE user32 gdi32)
endif()