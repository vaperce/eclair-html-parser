cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

project(eclair-html-parser)

if (WIN32)
  set(PATCH_BIN "C:/Program Files/Git/usr/bin/patch.exe")

  set(ICU_BIN_URL_X86 https://github.com/unicode-org/icu/releases/download/release-65-1/icu4c-65_1-Win32-MSVC2017.zip)
  set(ICU_BIN_HASH_X86 38d2bd534b44141b376530fb79f9cb5a7f54d07cc2b1ab861dd5fe83bfd3848dc0d66e9ec6c5756b6314560a7f53077367159d60b81a9ba79a799e64314ea3ff)
  set(ICU_BIN_URL_X64 https://github.com/unicode-org/icu/releases/download/release-65-1/icu4c-65_1-Win64-MSVC2017.zip)
  set(ICU_BIN_HASH_X64 6fe614e6f3edd6af3079d53c742e9bb56f6506527a2d0ac5481f0b5a6083ad97231b9cd6713e97d548e40e678cc834dc2fbec7eed282710de5378e6fb880b1b3)

  if ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
    set(ICU_BIN_URL ${ICU_BIN_URL_X64})
    set(ICU_BIN_HASH ${ICU_BIN_HASH_X64})
  else ()
    set(ICU_BIN_URL ${ICU_BIN_URL_X86})
    set(ICU_BIN_HASH ${ICU_BIN_HASH_X86})
  endif ()

  externalproject_add(
    compact_enc_det_project
    GIT_REPOSITORY https://github.com/google/compact_enc_det.git
    PATCH_COMMAND ${PATCH_BIN} -p1 <SOURCE_DIR>/CMakeLists.txt < ${CMAKE_SOURCE_DIR}/tools/ced.patch
    BUILD_COMMAND ${CMAKE_VS_MSBUILD_COMMAND} <BINARY_DIR>/CED.vcxproj /p:configuration=Release /p:platform=${CMAKE_GENERATOR_PLATFORM}
    INSTALL_COMMAND ""
  )

  externalproject_add(
    icu4c_project
    URL ${ICU_BIN_URL}
    URL_HASH SHA512=${ICU_BIN_HASH}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

else ()
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(ICU_TARGET Linux)
    set(CED_CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=TRUE)
    set(ICU_CONFIG_COMMAND_PREFIX ${CMAKE_COMMAND} -E env CXXFLAGS=-fPIC)

  elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(ICU_TARGET MacOSX)
    set(CED_CMAKE_ARGS "")
    set(ICU_CONFIG_COMMAND_PREFIX "")

  else ()
    message(FATAL_ERROR "Unsupported OS ${CMAKE_SYSTEM_NAME} - cannot build")

  endif ()

  externalproject_add(
    compact_enc_det_project
    GIT_REPOSITORY https://github.com/google/compact_enc_det.git
    CMAKE_ARGS ${CED_CMAKE_ARGS}
    BUILD_COMMAND make ced
    INSTALL_COMMAND ""
  )

  externalproject_add(
    icu4c_project
    URL https://github.com/unicode-org/icu/releases/download/release-65-1/icu4c-65_1-src.tgz
    URL_HASH SHA512=8f1ef33e1f4abc9a8ee870331c59f01b473d6da1251a19ce403f822f3e3871096f0791855d39c8f20c612fc49cda2c62c06864aa32ddab2dbd186d2b21ce9139
    CONFIGURE_COMMAND
      ${ICU_CONFIG_COMMAND_PREFIX}
      sh <SOURCE_DIR>/source/runConfigureICU ${ICU_TARGET} --enable-shared=no --enable-static=yes --enable-tests=no prefix=<INSTALL_DIR>
    BUILD_COMMAND make
    INSTALL_COMMAND make install
  )

endif ()

externalproject_get_property(compact_enc_det_project SOURCE_DIR)
set(COMPACT_ENC_DET_PROJECT_INCLUDE_DIR ${SOURCE_DIR})
externalproject_get_property(compact_enc_det_project BINARY_DIR)
set(COMPACT_ENC_DET_PROJECT_DIR ${BINARY_DIR})

externalproject_get_property(icu4c_project SOURCE_DIR)
set(ICU4C_SOURCE_DIR ${SOURCE_DIR})
externalproject_get_property(icu4c_project INSTALL_DIR)
set(ICU4C_DIR ${INSTALL_DIR})

if (WIN32)
  set(COMPACT_ENC_DET_LIBRARIES ${COMPACT_ENC_DET_PROJECT_DIR}/lib/Release/ced.lib)
  if ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
    set(ICU_WIN_LIB_DIR lib64)
  else ()
    set(ICU_WIN_LIB_DIR lib)
  endif ()
  set(ICU4C_LIBRARIES
    ${ICU4C_SOURCE_DIR}/${ICU_WIN_LIB_DIR}/icudt.lib
    ${ICU4C_SOURCE_DIR}/${ICU_WIN_LIB_DIR}/icuin.lib
    ${ICU4C_SOURCE_DIR}/${ICU_WIN_LIB_DIR}/icuio.lib
    ${ICU4C_SOURCE_DIR}/${ICU_WIN_LIB_DIR}/icutu.lib
    ${ICU4C_SOURCE_DIR}/${ICU_WIN_LIB_DIR}/icuuc.lib
  )
  set(ICU4C_INCLUDE_DIR ${ICU4C_SOURCE_DIR}/include)

else ()
  set(COMPACT_ENC_DET_LIBRARIES ${COMPACT_ENC_DET_PROJECT_DIR}/lib/libced.a)
  set(ICU4C_LIBRARIES
    ${ICU4C_DIR}/lib/libicudata.a
    ${ICU4C_DIR}/lib/libicui18n.a
    ${ICU4C_DIR}/lib/libicuio.a
    ${ICU4C_DIR}/lib/libicutu.a
    ${ICU4C_DIR}/lib/libicuuc.a
  )
  set(ICU4C_INCLUDE_DIR ${ICU4C_DIR}/include)

endif ()

add_library(compact_enc_det STATIC IMPORTED)
set_property(
  TARGET compact_enc_det
  PROPERTY IMPORTED_LOCATION
  ${COMPACT_ENC_DET_LIBRARIES}
)
add_dependencies(compact_enc_det compact_enc_det_project)

add_library(icu4c STATIC IMPORTED)
set_property(
  TARGET icu4c
  PROPERTY IMPORTED_LOCATION
  ${ICU4C_LIBRARIES}
)
add_dependencies(icu4c icu4c_project)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(SOURCES
  src/tokenizer/entities.cpp
  src/node.cpp
  src/errors.cpp
  src/document.cpp
  src/tag_names.cpp
)

include_directories(
  include
  src
  ${ICU4C_INCLUDE_DIR}
  ${COMPACT_ENC_DET_PROJECT_INCLUDE_DIR}
)

add_library(eclair-html-parser STATIC ${SOURCES})
set_property(TARGET eclair-html-parser PROPERTY CXX_STANDARD 17)
target_link_libraries(eclair-html-parser icu4c compact_enc_det)

install(TARGETS eclair-html-parser DESTINATION lib)
install(FILES
  ${COMPACT_ENC_DET_LIBRARIES}
  ${ICU4C_LIBRARIES}
  DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include/)
