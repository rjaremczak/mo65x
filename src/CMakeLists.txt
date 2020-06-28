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
    ${LIB_DIR}/imgui/imgui.cpp
    ${LIB_DIR}/imgui/imgui_draw.cpp
    ${LIB_DIR}/imgui/imgui_widgets.cpp
    ${LIB_DIR}/imgui/imgui_demo.cpp
    ${LIB_DIR}/imgui-sfml/imgui-SFML.cpp
)

add_library(${APP_LIB} STATIC ${SOURCES})
target_link_libraries(${APP_LIB} PUBLIC
    fmt::fmt 
    sfml-graphics 
    pthread 
)

if(UNIX)
    target_link_libraries(${APP_LIB} PUBLIC OpenGL::GL)
endif()

target_include_directories(${APP_LIB} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIB_DIR}/imgui
    ${LIB_DIR}/imgui-sfml
)