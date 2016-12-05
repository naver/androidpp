cmake_minimum_required(VERSION 2.8.12)

project(android)

set(LIBRARY_PRODUCT_DIR "${CMAKE_BINARY_DIR}" CACHE PATH "Path to the directory where products will be copied to.")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
include(PlatformAll)
include(Platform${CMAKE_SYSTEM_NAME})

add_subdirectory(android)
add_subdirectory(platforms)
add_subdirectory(private)
