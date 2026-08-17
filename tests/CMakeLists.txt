cmake_minimum_required(VERSION 3.10.0)

# Find Vulkan package
find_package(Vulkan REQUIRED)

# Подключение Catch2
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.0
)

FetchContent_MakeAvailable(Catch2)

# Регистрируем тесты для CTest
include(Catch)

# Набор тестов для текущего configure preset:
# ALL | MEMORY | VULKAN_INSTANCE | VULKAN_PHYS_DEVICE
set(NH_TEST_SUITE "ALL" CACHE STRING "Test suite selector")
set_property(CACHE NH_TEST_SUITE PROPERTY STRINGS ALL MEMORY VULKAN_INSTANCE VULKAN_PHYS_DEVICE)

if(NOT NH_TEST_SUITE STREQUAL "ALL"
   AND NOT NH_TEST_SUITE STREQUAL "MEMORY"
   AND NOT NH_TEST_SUITE STREQUAL "VULKAN_INSTANCE"
   AND NOT NH_TEST_SUITE STREQUAL "VULKAN_PHYS_DEVICE")
  message(FATAL_ERROR "Unsupported NH_TEST_SUITE='${NH_TEST_SUITE}'")
endif()

# Общие исходники проекта для Vulkan/Window-тестов
set(VULKAN_PROJECT_SOURCES
  ${CMAKE_SOURCE_DIR}/src/vulkan/instance/HVinstance.cpp
  ${CMAKE_SOURCE_DIR}/src/vulkan/surface/HVsurface.cpp
  ${CMAKE_SOURCE_DIR}/src/vulkan/phys_device/HVphysicalDevice.cpp
  ${CMAKE_SOURCE_DIR}/src/window/Hwindow.cpp
)

# Модуль: memory
if(NH_TEST_SUITE STREQUAL "ALL" OR NH_TEST_SUITE STREQUAL "MEMORY")
  add_executable(tests_memory
    ${CMAKE_CURRENT_SOURCE_DIR}/HstackTests.cpp
  )

  target_include_directories(tests_memory PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(tests_memory PRIVATE Catch2::Catch2WithMain Vulkan::Vulkan)

  catch_discover_tests(tests_memory
    TEST_PREFIX "memory/"
  )
endif()

# Модуль: vulkan/instance
if(NH_TEST_SUITE STREQUAL "ALL" OR NH_TEST_SUITE STREQUAL "VULKAN_INSTANCE")
  add_executable(tests_vulkan_instance
    ${CMAKE_CURRENT_SOURCE_DIR}/HVinstanceExtensionsTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HVinstanceLayersTests.cpp
    ${VULKAN_PROJECT_SOURCES}
  )

  target_include_directories(tests_vulkan_instance PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(tests_vulkan_instance PRIVATE Catch2::Catch2WithMain Vulkan::Vulkan)

  catch_discover_tests(tests_vulkan_instance
    TEST_PREFIX "vulkan/instance/"
  )
endif()

# Модуль: vulkan/phys_device
if(NH_TEST_SUITE STREQUAL "ALL" OR NH_TEST_SUITE STREQUAL "VULKAN_PHYS_DEVICE")
  add_executable(tests_vulkan_phys_device
    ${CMAKE_CURRENT_SOURCE_DIR}/HphysicalDeviceTypeTests.cpp
    ${VULKAN_PROJECT_SOURCES}
  )

  target_include_directories(tests_vulkan_phys_device PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(tests_vulkan_phys_device PRIVATE Catch2::Catch2WithMain Vulkan::Vulkan)

  catch_discover_tests(tests_vulkan_phys_device
    TEST_PREFIX "vulkan/phys_device/"
  )
endif()
