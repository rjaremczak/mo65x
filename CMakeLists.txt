cmake_minimum_required(VERSION 3.10)
project(mo65x)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(main lib lib/imgui lib/imgui-sfml)

add_subdirectory(main)
add_subdirectory(test)
add_subdirectory(lib/googletest)
add_subdirectory(lib/fmt)

add_executable(${CMAKE_PROJECT_NAME} start.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_main)