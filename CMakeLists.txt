cmake_minimum_required(VERSION 3.20.3 FATAL_ERROR)

project(std-lib-impl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC
 "src/*.h"
 "src/*.cpp")

add_executable(exec ${SRC})