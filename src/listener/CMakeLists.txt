set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/Cellar/nlohmann_json/3.1.0/include") 

set(SOURCE_FILES listener.cpp)

add_executable(listener ${SOURCE_FILES})

