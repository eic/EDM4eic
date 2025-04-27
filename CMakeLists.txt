# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2022 Whitney Armstrong, Sylvester Joosten, Wouter Deconinck

cmake_minimum_required(VERSION 3.12)

project(EDM4EIC
  LANGUAGES CXX)

SET( ${PROJECT_NAME}_VERSION_MAJOR 8 )
SET( ${PROJECT_NAME}_VERSION_MINOR 2 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )
SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

# C++ standard
set(CMAKE_CXX_STANDARD 17 CACHE STRING "Set the C++ standard to be used")
if(NOT CMAKE_CXX_STANDARD MATCHES "17|20")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--- Declare options -----------------------------------------------------------
option(BUILD_DATA_MODEL "Run podio class generator yaml file" ON)

if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

include(GNUInstallDirs)

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
