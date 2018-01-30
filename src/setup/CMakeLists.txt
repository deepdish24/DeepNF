set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

set(SOURCE_FILES
    main.cpp
    Orchestrator.cpp
)

set(HEADERS
    Orchestrator.h
)

add_executable(setup ${SOURCE_FILES} ${HEADERS})