cmake_minimum_required(VERSION 3.7)
project(setup)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES main.cpp Orchestrator.cpp Orchestrator.h)
add_executable(setup ${SOURCE_FILES})