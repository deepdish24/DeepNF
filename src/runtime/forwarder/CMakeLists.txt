set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wno-unknown-pragmas -Wextra")

add_executable(tester tester.cpp)
add_executable(receiver tester.cpp)

add_dependencies(tester runtime)
add_dependencies(receiver runtime)
