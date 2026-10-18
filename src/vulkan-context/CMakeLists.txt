cmake_minimum_required(VERSION 3.16)

find_package(Vulkan REQUIRED COMPONENTS glslc)

# Поиск компилятора glslc
if(TARGET Vulkan::glslc)
    set(GLSL_COMPILER Vulkan::glslc)
elseif(Vulkan_GLSLC_EXECUTABLE)
    set(GLSL_COMPILER ${Vulkan_GLSLC_EXECUTABLE})
else()
    find_program(GLSL_COMPILER glslc HINTS $ENV{VULKAN_SDK}/bin "C:/libraries/VulkanSDK/Bin")
endif()

# Исходные файлы шейдеров
set(SHADER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_2d.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_2d.frag
)

set(COMPILED_SHADERS "")

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPV_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
    
    add_custom_command(
        OUTPUT ${SPV_OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/shaders"
        COMMAND ${GLSL_COMPILER} ${SHADER} -o ${SPV_OUT}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPV_OUT} "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv"
        DEPENDS ${SHADER}
        COMMENT "Compiling Vulkan GLSL shader: ${SHADER_NAME} -> ${SHADER_NAME}.spv"
        VERBATIM
    )
    list(APPEND COMPILED_SHADERS ${SPV_OUT})
endforeach()

# Цель сборки шейдеров
add_custom_target(compile_vulkan_shaders DEPENDS ${COMPILED_SHADERS})

add_library(vulkan-context STATIC vulkan-context.cpp)
add_dependencies(vulkan-context compile_vulkan_shaders)

target_include_directories(vulkan-context PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(vulkan-context PRIVATE
    SHADERS_DIR="${CMAKE_CURRENT_BINARY_DIR}/shaders"
)

target_link_libraries(vulkan-context PUBLIC
    windows
    Vulkan::Vulkan
)