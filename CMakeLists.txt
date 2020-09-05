cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

project(eclair-html-parser)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(ICU_TARGET Linux)
  set(CED_PATCH_COMMAND "")
  set(CED_CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=TRUE)
  set(ICU_CONFIG_ENV env CXXFLAGS=-fPIC)

elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(ICU_TARGET MacOSX)
  set(CED_PATCH_COMMAND "")
  set(CED_CMAKE_ARGS "")
  set(ICU_CONFIG_ENV "")

elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(ICU_TARGET "Cygwin/MSVC")
  set(CED_PATCH_COMMAND "${CMAKE_COMMAND} -E sed -i -e '9,12d' CMakeLists.txt")
  set(CED_CMAKE_ARGS "")
  set(ICU_CONFIG_ENV "")

else ()
  message(FATAL_ERROR "Unsupported OS ${CMAKE_SYSTEM_NAME} - cannot build")

endif ()

externalproject_add(
  compact_enc_det_project
  GIT_REPOSITORY https://github.com/google/compact_enc_det.git
  PATCH_COMMAND ${CED_PATCH_COMMAND}
  CMAKE_ARGS ${CED_CMAKE_ARGS}
  BUILD_COMMAND make ced
  INSTALL_COMMAND ""
)

externalproject_get_property(compact_enc_det_project SOURCE_DIR)
set(COMPACT_ENC_DET_PROJECT_INCLUDE_DIR ${SOURCE_DIR})
externalproject_get_property(compact_enc_det_project BINARY_DIR)
set(COMPACT_ENC_DET_PROJECT_DIR ${BINARY_DIR})

add_library(compact_enc_det STATIC IMPORTED)
set(COMPACT_ENC_DET_LIBRARIES ${COMPACT_ENC_DET_PROJECT_DIR}/lib/libced.a)
set_property(
  TARGET compact_enc_det
  PROPERTY IMPORTED_LOCATION
  ${COMPACT_ENC_DET_LIBRARIES}
)
add_dependencies(compact_enc_det compact_enc_det_project)

externalproject_add(
  icu4c_project
  URL https://github.com/unicode-org/icu/releases/download/release-65-1/icu4c-65_1-src.tgz
  URL_HASH SHA512=8f1ef33e1f4abc9a8ee870331c59f01b473d6da1251a19ce403f822f3e3871096f0791855d39c8f20c612fc49cda2c62c06864aa32ddab2dbd186d2b21ce9139
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -E ${ICU_CONFIG_ENV}
      sh <SOURCE_DIR>/source/runConfigureICU ${ICU_TARGET} --enable-shared=no --enable-static=yes --enable-tests=no prefix=<INSTALL_DIR>
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

externalproject_get_property(icu4c_project INSTALL_DIR)
set(ICU4C_DIR ${INSTALL_DIR})

add_library(icu4c STATIC IMPORTED)
set(ICU4C_LIBRARIES
  ${ICU4C_DIR}/lib/libicudata.a
  ${ICU4C_DIR}/lib/libicui18n.a
  ${ICU4C_DIR}/lib/libicuio.a
  ${ICU4C_DIR}/lib/libicutest.a
  ${ICU4C_DIR}/lib/libicutu.a
  ${ICU4C_DIR}/lib/libicuuc.a
)
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
  ${ICU4C_DIR}/include
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
