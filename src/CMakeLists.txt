set(CMAKE_MACOSX_BUNDLE ON)
set(CMAKE_WIN32_EXECUTABLE ON)

if(WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

if(UNIX)
    add_definitions(-DGL_SILENCE_DEPRECATION)
    find_package(OpenGL REQUIRED)
endif()

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

file(GLOB_RECURSE SOURCES *.h* *.c*)
list(REMOVE_ITEM SOURCES main.cpp)
list(APPEND SOURCES 
../lib/imgui/imgui.cpp
../lib/imgui/imgui_draw.cpp
../lib/imgui/imgui_widgets.cpp
../lib/imgui/imgui_demo.cpp
../lib/imgui-sfml/imgui-SFML.cpp
)

add_library(${APP_LIB} STATIC ${SOURCES})
target_link_libraries(${APP_LIB} 
fmt::fmt 
sfml-graphics 
pthread 
)

if(UNIX)
    target_link_libraries(${APP_LIB} OpenGL::GL)
endif()

target_include_directories(${APP_LIB} PUBLIC
.
../lib/imgui
../lib/imgui-sfml
)

add_executable(${APP} main.cpp)
target_link_libraries(${APP} ${APP_LIB})