# Catch flavoured testing
set(CATCH_INCLUDE_DIRS "${CONSTRUCTION_EXTERNAL_DIRECTORY}/catch")
include_directories(${CATCH_INCLUDE_DIRS})

enable_testing(true)

add_executable(testing main.cpp)
target_link_libraries(testing tensor ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME runtesting
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:testing>
    -DSOURCEDIR=${CMAKE_CURRENT_SOURCE_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/runtest.cmake
)
