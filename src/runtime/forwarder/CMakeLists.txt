set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wno-unknown-pragmas -Wextra")

add_executable(forwarder forwarder.cpp)
add_executable(receiver receiver.cpp)

add_dependencies(forwarder runtime)
add_dependencies(receiver runtime)
