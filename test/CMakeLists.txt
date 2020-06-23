set(TEST_BINARY ${CMAKE_PROJECT_NAME}_test)

#include_directories(.)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h* *.c*)

add_executable(${TEST_BINARY} ${TEST_SOURCES})
add_test(NAME ${TEST_BINARY} COMMAND ${TEST_BINARY})
target_link_libraries(${TEST_BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest)