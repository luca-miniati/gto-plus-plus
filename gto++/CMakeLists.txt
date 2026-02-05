cmake_minimum_required(VERSION 3.16)
project(gtoplusplus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(gtoplusplus
    src/solver.cpp
)

target_include_directories(gtoplusplus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
