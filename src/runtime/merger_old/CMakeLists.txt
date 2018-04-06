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
<<<<<<< HEAD
=======

 
>>>>>>> 8c611b2f0ed616771ecc55a0b886025b5e869727
