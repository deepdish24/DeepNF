set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SOURCE_FILES
    configure.cpp
)

#set(HEADERS
    # add headers files here eventually
    #       ../merger_old/MergerOperator.cpp ../merger_old/MergerOperator.h ../merger_old/ActionTableHelper.cpp ../merger_old/ActionTableHelper.h)

add_executable(configure ${SOURCE_FILES} ${HEADERS})
target_link_libraries(configure common merger_lib)
