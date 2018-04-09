set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(fw dnf_firewall.cpp)

target_link_libraries(fw runtime)