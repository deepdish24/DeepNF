set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
    RuntimeNode.cpp
    Machine.cpp
    ServiceGraphUtil.cpp
    MachineConfigurator.cpp
        )

set(HEADERS
    # add headers files here eventually
    RuntimeNode.h
    Machine.h
    MachineConfigurator.h
    NF.h
    ServiceGraphUtil.h
        )

set(PROTOCOL_BUFFERS
        ${PROTO_DIR}/Machine.pb.cc
        ${PROTO_DIR}/Machine.pb.h
     )
set_source_files_properties(${PROTOCOL_BUFFERS} PROPERTIES GENERATED TRUE)


add_subdirectory(sgu_test)

add_library(common ${SOURCE_FILES} ${HEADERS} ${PROTOCOL_BUFFERS})

set_source_files_properties(${PROTOCOL_BUFFERS} PROPERTIES GENERATED TRUE)
target_link_libraries(common protobuf)
add_dependencies(common protocol_buffers)
