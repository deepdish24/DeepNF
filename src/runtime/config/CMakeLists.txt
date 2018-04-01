set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
    configure.cpp
)

set(HEADERS
    # add headers files here eventually
        ../merger/MergerOperator.cpp ../merger/MergerOperator.h ../merger/ActionTableHelper.cpp ../merger/ActionTableHelper.h)

add_executable(configure ${SOURCE_FILES} ${HEADERS})
target_link_libraries(configure common)