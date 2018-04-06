set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/ubuntu/.linuxbrew/Cellar/nlohmann_json/3.1.2/include") 

set(SOURCE_FILES
    configure.cpp
)


set(HEADERS
    
)


add_executable(configure ${SOURCE_FILES} ${HEADERS})
target_link_libraries(configure common merger_lib)
