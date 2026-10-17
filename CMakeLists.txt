cmake_minimum_required(VERSION 3.10)
project(Horizons VERSION 0.1.0 LANGUAGES C CXX)

# Для P2996 нужен черновик стандарта C++26 (-std=c++2c)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

enable_testing()
# Find Vulkan package
find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
	message(FATAL_ERROR "Vulkan SDK not found. Please install the Vulkan SDK and set the VULKAN_SDK environment variable.")
endif()

if (WIN32)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif()

add_subdirectory(src)

# Unit tests
add_subdirectory(tests)
