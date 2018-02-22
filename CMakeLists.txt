project (DeepNF)
cmake_minimum_required (VERSION 3.7)

set (DeepNF_VERSION_MAJOR 1)
set (DeepNF_VERSION_MINOR 0)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/Cellar/nlohmann_json/3.1.0/include") 

include(ExternalProject)
ExternalProject_Add(google_protobuf
  GIT_REPOSITORY https://github.com/google/protobuf.git
  GIT_TAG v3.4.1
  CONFIGURE_COMMAND ${CMAKE_COMMAND}
    -G${CMAKE_GENERATOR} ${CMAKE_CURRENT_BINARY_DIR}/google_protobuf-prefix/src/google_protobuf/cmake
    -Dprotobuf_BUILD_TESTS:BOOL=OFF
    -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)
set_target_properties(google_protobuf PROPERTIES EXCLUDE_FROM_ALL TRUE)

find_library(protobuf_lib NAMES protobuf libprotobuf)
find_library(protobufd_lib NAMES protobufd libprotobufd)

add_subdirectory(src/common)
add_subdirectory(src/listener)
add_subdirectory(src/runtime)
add_subdirectory(src/setup)
