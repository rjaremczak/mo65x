cmake_minimum_required(VERSION 3.9)

project(mo65x)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)

#include_directories(./)
#include_directories(test)

file(GLOB_RECURSE APP_SOURCES "*.c*" "*.h")
file(GLOB_RECURSE TEST_SOURCES "test/*.c*" "test/*.h")

list(REMOVE_ITEM TEST_SOURCES "test/main.cpp")

add_executable(mo65x ${APP_SOURCES} ${TEST_SOURCES})

target_link_libraries(mo65x
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Test
    pthread
)
