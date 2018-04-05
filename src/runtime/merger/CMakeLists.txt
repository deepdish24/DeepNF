set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wno-unknown-pragmas -Wextra")

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
add_executable(merger ${SOURCE_FILES} ${HEADERS})

add_dependencies(merger common)
