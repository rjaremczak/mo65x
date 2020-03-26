cmake_minimum_required(VERSION 3.9)

project(mo65x)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set ( UIS
 mainwindow.ui
)

set ( RESOURCES
 main.qrc
)
set(SFML_STATIC_LIBRARIES TRUE)

if(RUN_TESTS)
    add_definitions(-DRUN_TESTS)
endif(RUN_TESTS)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)

add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)
add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)

include_directories(src lib lib/imgui lib/imgui-sfml)

set(LIB_SOURCES
    lib/nlohmann/json.hpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui-sfml/imgui-SFML.cpp
    )

file(GLOB_RECURSE APP_SOURCES "src/*.c*" "src/*.h*")
file(GLOB_RECURSE TEST_SOURCES "test/*.c*" "test/*.h*")
file(GLOB_RECURSE UIS "src/*.ui")
file(GLOB_RECURSE RESOURCES "src/*.qrc")

add_executable(mo65x ${APP_SOURCES} ${LIB_SOURCES} ${UIS} ${RESOURCES} ${TEST_SOURCES})

target_link_libraries(mo65x
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    fmt::fmt
    sfml-graphics
    pthread
    gtest
)
