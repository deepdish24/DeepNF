set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wno-unknown-pragmas -Wextra")

add_executable(tester tester.cpp)
add_executable(forwarder forwarder.cpp)
add_executable(null_tester null)tester.cpp)

target_link_libraries(tester runtime)
target_link_libraries(forwarder runtime)
target_link_libraries(null_tester runtime)
