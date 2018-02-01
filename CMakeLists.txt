project (DeepNF)
cmake_minimum_required (VERSION 3.7)

set (DeepNF_VERSION_MAJOR 1)
set (DeepNF_VERSION_MINOR 0)

add_subdirectory(src/common)
add_subdirectory(src/listener)
add_subdirectory(src/runtime)
add_subdirectory(src/setup)