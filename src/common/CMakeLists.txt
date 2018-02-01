set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
    Node.cpp
    Machine.cpp
    ServiceGraph.cpp
)

set(HEADERS
    # add headers files here eventually
    Node.h
    Machine.h
    ServiceGraph.h
    NF.h
)

add_library(common ${SOURCE_FILES} ${HEADERS})