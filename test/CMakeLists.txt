file(GLOB_RECURSE SOURCES *.h* *.c*)

add_executable(${APP_TEST} ${SOURCES})
target_link_libraries(${APP_TEST} ${APP_LIB} gtest_main)

add_test(NAME ${APP_TEST} COMMAND ${APP_TEST})