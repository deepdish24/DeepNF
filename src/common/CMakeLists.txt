
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
    configure.cpp
    NF.cpp
    Node.cpp
)

set(HEADERS
    # add headers files here eventually
)

add_executable(runtime ${SOURCE_FILES} ${HEADERS})