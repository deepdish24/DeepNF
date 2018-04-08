set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCE_FILES
        address_util.cpp
        packet.cpp
        socket_util.cpp
        )

set(HEADERS
        address.h
        address_util.h
        packet.h
        socket_util.h
        )

add_library(runtime ${SOURCE_FILES} ${HEADERS})

add_subdirectory(merger)
add_subdirectory(config)
add_subdirectory(forwarder)
add_subdirectory(nf_configs)
