project (DeepNF)
cmake_minimum_required (VERSION 3.7)

set (DeepNF_VERSION_MAJOR 1)
set (DeepNF_VERSION_MINOR 0)

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

file(GLOB PROTOCOL_BUFFERS src/proto/*.proto)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(PROTO_DIR ${GENERATED_DIR}/src/proto)

add_custom_target(protocol_buffers ALL SOURCES ${PROTOCOL_BUFFERS})
add_custom_command(TARGET protocol_buffers PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_DIR})
foreach(PROTOCOL_BUFFER ${PROTOCOL_BUFFERS})
    add_custom_command(
            TARGET protocol_buffers
            COMMAND ${CMAKE_BINARY_DIR}/bin/protoc -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${GENERATED_DIR} ${PROTOCOL_BUFFER}
    )
endforeach()

include_directories(${GENERATED_DIR} ${CMAKE_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories("usr/local/include/google/protobuf")
link_directories(${CMAKE_BINARY_DIR}/lib)
#link_directories(${CMAKE_BINARY_DIR}/lib64)

find_library(protobuf_lib NAMES protobuf libprotobuf)
find_library(protobufd_lib NAMES protobufd libprotobufd)

add_subdirectory(src/common)
add_subdirectory(src/listener)
add_subdirectory(src/runtime)
add_subdirectory(src/setup)
