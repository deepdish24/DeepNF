set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#add_executable(sender pkt_sender.cpp)
add_executable(receiver pkt_receiver.cpp)

#target_link_libraries(sender runtime)
target_link_libraries(receiver runtime)