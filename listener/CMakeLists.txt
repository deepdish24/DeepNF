cmake_minimum_required(VERSION 3.7)
project(listener)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES listener.cpp)
add_executable(listener ${SOURCE_FILES})