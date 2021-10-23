cmake_minimum_required(VERSION 3.15)

set( BGFX_OPENGL_VERSION   "41"       CACHE STRING "Specify minimum opengl version" )

project(sdl-bgfx-imgui-starter LANGUAGES CXX)

find_package(SDL2 REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(bgfx REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(imgui REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(libtcc REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)


set(CMAKE_CXX_FLAGS "-fpermissive")

include_directories("third-party/build/cmakefied-prefix/src/imgui")
include_directories("src")
include_directories("src/window")
include_directories("src/ui")
include_directories("src/render")
include_directories("glm")
include_directories("src/ui/ax")


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp sdl-imgui/imgui_impl_sdl.cpp
                                       bgfx-imgui/imgui_impl_bgfx.cpp)                        
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static SDL2::SDL2main
                                              bgfx::bgfx bgfx::bx imgui::imgui)

target_link_libraries(${PROJECT_NAME} PRIVATE libtcc)

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
