cmake_minimum_required(VERSION 3.16)

add_library(text-document-viewer STATIC text-document-viewer.cpp)

target_include_directories(text-document-viewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)