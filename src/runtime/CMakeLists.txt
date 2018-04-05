set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
        address.h
        address_util.h
        packet.h
        socket_util.h
        )

set(HEADERS
        address_util.cpp
        packet.cpp
        socket_util.cpp
        )

add_library(runtime ${SOURCE_FILES} ${HEADERS})

add_subdirectory(./merger)
add_subdirectory(./config)
