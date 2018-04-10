set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -g")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/ubuntu/.linuxbrew/Cellar/nlohmann_json/3.1.2/include")

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
target_link_libraries(runtime merger_lib)
target_link_libraries(setup common runtime merger_lib)
