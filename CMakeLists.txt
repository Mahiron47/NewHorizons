cmake_minimum_required(VERSION 3.16)

project(horizon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)


add_subdirectory(src/vulkan-context)
add_subdirectory(src/windows)
add_subdirectory(src/text-document-viewer)
add_subdirectory(src/text-pronounce-handle)

add_executable(horizon src/main.cpp)

target_link_libraries(horizon PRIVATE windows
                                      vulkan-context
                                      text-document-viewer
                                      text-pronounce-handle)