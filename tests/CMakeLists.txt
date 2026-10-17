cmake_minimum_required(VERSION 3.10.0)

# Подключение Catch2
include(FetchContent)

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                            GIT_TAG        v3.5.0)

FetchContent_MakeAvailable(Catch2)

# Регистрируем тесты для CTest
include(Catch)

add_subdirectory(h_memory)
add_subdirectory(vk_instance)
add_subdirectory(vk_phys_device)
add_subdirectory(vk_device)