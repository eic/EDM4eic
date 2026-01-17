# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2022 Whitney Armstrong, Sylvester Joosten, Wouter Deconinck

cmake_minimum_required(VERSION 3.12)

project(EDM4EIC
  LANGUAGES CXX)

SET( ${PROJECT_NAME}_VERSION_MAJOR 8 )
SET( ${PROJECT_NAME}_VERSION_MINOR 6 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )
SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

# Verify that schema version in edm4eic.yaml matches project version
# Schema version is defined as 100*major + minor (patch is schema-invariant)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/edm4eic.yaml" EDM4EIC_YAML_CONTENT)
string(REGEX MATCH "schema_version:[ \t]*([0-9]+)" SCHEMA_VERSION_MATCH "${EDM4EIC_YAML_CONTENT}")
if(SCHEMA_VERSION_MATCH)
  set(SCHEMA_VERSION ${CMAKE_MATCH_1})
  math(EXPR EXPECTED_SCHEMA_VERSION "${${PROJECT_NAME}_VERSION_MAJOR} * 100 + ${${PROJECT_NAME}_VERSION_MINOR}")
  if(NOT SCHEMA_VERSION EQUAL EXPECTED_SCHEMA_VERSION)
    message(FATAL_ERROR
      "Schema version mismatch: edm4eic.yaml has schema_version=${SCHEMA_VERSION}, "
      "but CMakeLists.txt version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH} "
      "requires schema_version=${EXPECTED_SCHEMA_VERSION}. "
      "Please update schema_version in edm4eic.yaml to match the project version.")
  endif()
else()
  message(WARNING "Could not find schema_version in edm4eic.yaml")
endif()

# C++ standard
set(CMAKE_CXX_STANDARD_MIN 17)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "Set the C++ standard to be used")
if(CMAKE_CXX_STANDARD LESS CMAKE_CXX_STANDARD_MIN)
  message(
    FATAL_ERROR
    "Unsupported C++ standard: ${CMAKE_CXX_STANDARD} (at least ${CMAKE_CXX_STANDARD_MIN} required)"
  )
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--- Declare options -----------------------------------------------------------
option(BUILD_DATA_MODEL "Run podio class generator yaml file" ON)

if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

include(GNUInstallDirs)

include(CTest)

find_package(podio 0.15)
if(NOT podio_FOUND)
  find_package(podio 1.0 REQUIRED)
endif()
include_directories(${podio_INCLUDE_DIR})

find_package(EDM4HEP 0.10.3 REQUIRED)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT 6 REQUIRED COMPONENTS Core RIO Tree)
include(${ROOT_USE_FILE})

PODIO_GENERATE_DATAMODEL(edm4eic edm4eic.yaml headers sources
  UPSTREAM_EDM edm4hep:${EDM4HEP_DATA_DIR}/edm4hep.yaml
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
  )

PODIO_ADD_DATAMODEL_CORE_LIB(edm4eic "${headers}" "${sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)
# It's our responsibility to link to upstream datamodel
target_link_libraries(edm4eic PUBLIC EDM4HEP::edm4hep)

PODIO_ADD_ROOT_IO_DICT(edm4eicDict edm4eic "${headers}" src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(edm4eic::edm4eicDict ALIAS edm4eicDict )

list(APPEND EDM4EIC_INSTALL_LIBS edm4eic edm4eicDict)

add_subdirectory(utils)
add_subdirectory(test)

install(TARGETS ${EDM4EIC_INSTALL_LIBS}
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/edm4eic"
  COMPONENT dev)

install(FILES
  "${PROJECT_BINARY_DIR}/edm4eicDictDict.rootmap"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev)

install(FILES
  edm4eic.yaml
  DESTINATION "${CMAKE_INSTALL_DATADIR}/edm4eic" COMPONENT dev)

install(FILES
  "${PROJECT_BINARY_DIR}/libedm4eicDict_rdict.pcm"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev)

# -------------------------
# install class documentation

file(RELATIVE_PATH CMAKE_CURRENT_BINARY_RELATIVE_DIR
  ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
configure_file(
  ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  @ONLY)

option(GENERATE_DOCS "Generate the doxygen base API reference" ON)

if (GENERATE_DOCS)
  find_package(Doxygen COMPONENTS dot)
  if(DOXYGEN_FOUND)
    add_custom_target(docs ALL
      DEPENDS edm4eic.yaml ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
  else()
    message(STATUS "Doxygen not found; no documentation will be built.")
  endif()
endif()

# -------------------------
# add version files

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EDM4eicVersion.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/EDM4eicVersion.h )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EDM4eicVersion.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/edm4eic )

# -------------------------
# install library config
include(CMakePackageConfigHelpers)

# Install and export targets
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/${PROJECT_NAME}
  )

set(TARGETS_INSTALL_PATH lib/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake)

configure_package_config_file(
  cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION lib/${PROJECT_NAME}
  PATH_VARS TARGETS_INSTALL_PATH
  )

write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${${PROJECT_NAME}_VERSION}
  COMPATIBILITY AnyNewerVersion
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/${PROJECT_NAME}
  )
