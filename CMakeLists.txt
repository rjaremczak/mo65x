cmake_minimum_required(VERSION 3.10)
project(mo65x)

enable_testing()

set(CMAKE_MACOSX_BUNDLE ON)
set(CMAKE_WIN32_EXECUTABLE ON)

set(APP ${CMAKE_PROJECT_NAME})
set(APP_LIB ${APP}_lib)
set(APP_TEST ${APP}_test)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)
add_subdirectory(lib/fmt)

add_executable(${APP} src/main.cpp)
target_link_libraries(${APP} ${APP_LIB})