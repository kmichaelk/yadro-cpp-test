cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME cclub)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_LIBRARY "${PROJECT_NAME}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" gtest)

add_subdirectory(src)
add_subdirectory(bin)
add_subdirectory(test)