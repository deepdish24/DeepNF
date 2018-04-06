set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I /usr/include/pcap -lpcap")

set(SOURCE_FILES
    merge.cpp
)

set(HEADERS
    # add headers files here eventually
)

add_executable(merge ${SOURCE_FILES} ${HEADERS})
