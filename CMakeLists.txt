cmake_minimum_required(VERSION 3.10)
project(mo65x)

enable_testing()

set(APP ${CMAKE_PROJECT_NAME})
set(APP_LIB ${APP}_lib)
set(APP_TEST ${APP}_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)
add_subdirectory(lib/fmt)