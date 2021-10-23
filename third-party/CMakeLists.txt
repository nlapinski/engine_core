cmake_minimum_required(VERSION 3.15)

project(third-party)

include(ExternalProject)

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT isMultiConfig)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Debug
        CACHE STRING "" FORCE)
  endif ()
  set(build_type_dir ${CMAKE_BUILD_TYPE})
  set(build_type_arg -DCMAKE_BUILD_TYPE=$<CONFIG>)
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set(THIRD_PARTY_CMAKE_COMMAND emcmake cmake)
else ()
  set(THIRD_PARTY_CMAKE_COMMAND ${CMAKE_COMMAND})
endif ()

ExternalProject_Add(
  libtcc
  GIT_REPOSITORY https://github.com/nlapinski/libtcc-cmake.git
  GIT_TAG master
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libtcc/build/${build_type_dir}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_COMMAND ${THIRD_PARTY_CMAKE_COMMAND}
  CMAKE_ARGS ${build_type_arg} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
             "$<$<CONFIG:Debug>:-DCMAKE_DEBUG_POSTFIX=d>"
             -DLIBTCC_INSTALL_RUNTIME=TRUE
             -DLIBTCC_RUNTIME_PATH=TRUE
             -DLIBTCC_TARGET_SPECIFIC_NAME=TRUE
  BUILD_COMMAND cmake --build <BINARY_DIR>
  INSTALL_COMMAND cmake --build <BINARY_DIR> --target install)


ExternalProject_Add(
  SDL2
  URL https://www.libsdl.org/release/SDL2-2.0.14.tar.gz
  URL_HASH MD5=76ed4e6da9c07bd168b2acd9bfefab1b
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/SDL2/build/${build_type_dir}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_COMMAND ${THIRD_PARTY_CMAKE_COMMAND}
  CMAKE_ARGS ${build_type_arg} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  BUILD_COMMAND cmake --build <BINARY_DIR>
  INSTALL_COMMAND cmake --build <BINARY_DIR> --target install)

ExternalProject_Add(
  bgfx
  GIT_REPOSITORY https://github.com/nlapinski/bgfx.cmake.git
  GIT_TAG 5e7ec5f7d783fb348608abc6f932a34b937f0115
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bgfx/build/${build_type_dir}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_COMMAND ${THIRD_PARTY_CMAKE_COMMAND}
  CMAKE_ARGS ${build_type_arg} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
             "$<$<CONFIG:Debug>:-DCMAKE_DEBUG_POSTFIX=d>"
             -DBGFX_BUILD_EXAMPLES=OFF
             -DBGFX_OPENGL_VERSION=41
  BUILD_COMMAND cmake --build <BINARY_DIR>
  INSTALL_COMMAND cmake --build <BINARY_DIR> --target install)

ExternalProject_Add(
  cmakefied
  GIT_REPOSITORY https://github.com/nlapinski/cmakefied.git
  GIT_TAG 3c5e1b964a3aa5616879ae5220214e11903dcb18
  UPDATE_COMMAND cd <SOURCE_DIR> && bash git-clone.sh imgui && cd
                 ${CMAKE_SOURCE_DIR}
  SOURCE_SUBDIR ../imgui
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/imgui/build/${build_type_dir}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_COMMAND ${THIRD_PARTY_CMAKE_COMMAND}
  CMAKE_ARGS ${build_type_arg} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
             -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=ON
  BUILD_COMMAND cmake --build <BINARY_DIR>
  INSTALL_COMMAND cmake --build <BINARY_DIR> --target install)


  
ExternalProject_Add(
  glm
  GIT_REPOSITORY https://github.com/nlapinski/glm.git
  GIT_TAG 6ad79aae3eb5bf809c30bf1168171e9e55857e45
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/glm/build/${build_type_dir}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_COMMAND ${THIRD_PARTY_CMAKE_COMMAND}
  CMAKE_ARGS ${build_type_arg} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
             "$<$<CONFIG:Debug>:-DCMAKE_DEBUG_POSTFIX=d>"
  BUILD_COMMAND cmake --build <BINARY_DIR>
  INSTALL_COMMAND cmake --build <BINARY_DIR> --target install)



