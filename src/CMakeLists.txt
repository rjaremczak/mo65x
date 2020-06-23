set(CMAKE_MACOSX_BUNDLE ON)

if(WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

if(UNIX)
    add_definitions(-DGL_SILENCE_DEPRECATION)
    find_package(OpenGL REQUIRED)
endif()

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

#include_directories(../lib/imgui-sfml)

file(GLOB_RECURSE APP_SOURCES LIST_DIRECTORIES true *.h* *.c*)

set(LIB_SOURCES
    ../lib/nlohmann/json.hpp
    ../lib/imgui/imgui.cpp
    ../lib/imgui/imgui_draw.cpp
    ../lib/imgui/imgui_widgets.cpp
    ../lib/imgui/imgui_demo.cpp
    ../lib/imgui-sfml/imgui-SFML.cpp
    )

set(SOURCES ${APP_SOURCES} ${LIB_SOURCES})

add_executable(${CMAKE_PROJECT_NAME}_run ${SOURCES})
add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCES})

set(LINK_LIBRARIES 
    fmt::fmt 
    sfml-graphics 
    pthread 
    gtest
    )

target_link_libraries(${CMAKE_PROJECT_NAME}_run ${LINK_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME}_lib ${LINK_LIBRARIES})

if(UNIX)
    target_link_libraries(OpenGL::GL)
endif()
