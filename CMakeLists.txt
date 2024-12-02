cmake_minimum_required(VERSION 3.29)
project(sistemasembebidos C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/includes)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/drivers)
include_directories(${PROJECT_SOURCE_DIR}/FreeRTOS)


add_executable(sistemasembebidos "main.c" startup.c)
