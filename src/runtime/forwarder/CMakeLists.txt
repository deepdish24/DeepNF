set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wno-unknown-pragmas -Wextra")

set(SOURCE_FILES
        tester.cpp
        )

add_executable(tester)
#add_executable(forwarder forwarder.cpp)

add_dependencies(tester runtime)
#add_dependencies(forwarder runtime)
