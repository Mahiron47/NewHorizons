cmake_minimum_required(VERSION 3.10)
project(Horizons VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Find Vulkan package
find_package(Vulkan REQUIRED)

# Create your executable
add_executable(Horizons src/main.cpp
    					src/window/Hwindow.cpp
    					src/threads/HrenderThread.cpp
    					src/vulkan/surface/HVsurface.cpp
    					src/vulkan/instance/HVinstance.cpp
    					src/vulkan/instance/extensions/HVinstanceExtensions.cpp
    					src/vulkan/device/HVdevice.cpp
						src/vulkan/device/extensions/HVdeviceExtensions.cpp
						src/vulkan/device/layers/HVdeviceLayers.cpp
						src/Horizon.cpp
)

# Include directories
target_include_directories(Horizons PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable Unicode on Windows
target_compile_definitions(Horizons PRIVATE UNICODE _UNICODE)

# Link against Vulkan
target_link_libraries(Horizons PUBLIC Vulkan::Vulkan)

# Link CTests
include(CTest)
enable_testing()

# Unit tests
add_subdirectory(tests)
