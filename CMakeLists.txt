cmake_minimum_required(VERSION 3.29)
project(sistemasembebidos C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/includes)

add_executable(sistemasembebidos main.c)
