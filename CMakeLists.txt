cmake_minimum_required(VERSION 3.10)
project(mo65x)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src lib lib/imgui lib/imgui-sfml)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)
add_subdirectory(lib/fmt)