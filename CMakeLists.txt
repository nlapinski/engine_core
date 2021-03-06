cmake_minimum_required(VERSION 3.15)

set( BGFX_OPENGL_VERSION   "41"       CACHE STRING "Specify minimum opengl version" )

project(engine_core LANGUAGES CXX)

find_package(SDL2 REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(bgfx REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(imgui REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(libtcc REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(glm REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)

include_directories("third-party/build/libtcc-prefix/src/libtcc/src")
include_directories("third-party/build/libtcc-prefix/src/include/tcc")
include_directories("third-party/build/cmakefied-prefix/src/imgui")
include_directories("third-party/build/include")


set(CMAKE_CXX_FLAGS "-fpermissive")

add_subdirectory(modules/imgui-node-editor/NodeEditor)
include_directories("modules/imgui-node-editor/NodeEditor/Include")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/modules/imgui-node-editor/Support/CmakeModules)
find_package(imgui_node_editor REQUIRED)

include_directories("third-party/build/cmakefied-prefix/src/imgui")
include_directories("src")
include_directories("src/window")
include_directories("src/ui")
include_directories("src/render")
include_directories("glm")



add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp src/ui/ax/Widgets.cpp src/ui/ax/Drawing.cpp src/ui/ax/Builders.cpp src/application.cpp src/window/jgl_window.cpp src/ui/property_panel.cpp src/ui/scene_view.cpp src/render/ui_context.cpp src/render/opengl_context.cpp sdl-imgui/imgui_impl_sdl.cpp
                                       bgfx-imgui/imgui_impl_bgfx.cpp)                             
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static SDL2::SDL2main bgfx::bgfx bgfx::bx imgui::imgui)

target_link_libraries(${PROJECT_NAME} PRIVATE libtcc_x86_64)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui_node_editor)


set_target_properties(
  ${PROJECT_NAME}
  # required for project when using visual studio generator
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if (WIN32)
  # copy the SDL2.dll to the same folder as the executable
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)
endif ()


if (MINGW)
  # copy the libtcc1-x86_64.a to the same folder as the executable
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)
endif ()


if (MINGW)
target_link_options(${PROJECT_NAME} PRIVATE "-lmingw32")
endif()
if (MINGW)
target_link_options(${PROJECT_NAME} PRIVATE "-mconsole")
endif()


set(CMAKE_CXX_FLAGS -fpermissive)
