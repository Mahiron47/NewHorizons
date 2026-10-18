cmake_minimum_required(VERSION 3.16)

add_library(text-pronounce-handle STATIC text-pronounce-handle.cpp)

target_include_directories(text-pronounce-handle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)