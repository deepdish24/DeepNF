set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/Cellar/nlohmann_json/3.1.0/include") 

set(SOURCE_FILES
    main.cpp
    Orchestrator.cpp
    ServiceGraphNode.cpp
)

set(HEADERS
    Orchestrator.h
    ServiceGraphNode.h
)

add_executable(setup ${SOURCE_FILES} ${HEADERS})
target_link_libraries(setup common)
