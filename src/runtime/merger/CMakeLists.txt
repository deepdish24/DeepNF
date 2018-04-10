set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter")

set(SOURCE_FILES
    merger.cpp
    ActionTable.cpp
    MergerInfo.cpp
    ConflictItem.cpp
    MergerOperator.cpp
        )

set(HEADERS
    ActionTable.h
    MergerInfo.h
    ConflictItem.h
    MergerOperator.h
        )

add_library(merger_lib ${SOURCE_FILES} ${HEADERS})
target_link_libraries(merger_lib common)
target_link_libraries(merger_lib runtime)

add_dependencies(merger_lib runtime)
add_dependencies(merger_lib common)

add_executable(merger ${SOURCE_FILES} ${HEADERS})

target_link_libraries(merger common)
target_link_libraries(merger runtime)
