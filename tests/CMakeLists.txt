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

# Собираем тестовые исходники
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Исходники проекта которые нужны для тестов
set(PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/vulkan/instance/HVinstance.cpp
    ${CMAKE_SOURCE_DIR}/src/vulkan/surface/HVsurface.cpp
    ${CMAKE_SOURCE_DIR}/src/vulkan/phys_device/HVphysicalDevice.cpp
    ${CMAKE_SOURCE_DIR}/src/window/Hwindow.cpp
)

# Создаём исполняемый файл тестов
add_executable(tests ${TEST_SOURCES} ${PROJECT_SOURCES})

# Пути к заголовкам проекта
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Линкуем Catch2 и Vulkan
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain Vulkan::Vulkan)

# Регистрируем тесты для CTest
include(Catch)
catch_discover_tests(tests)
