cmake_minimum_required(VERSION 3.12)
project(EICD 
  VERSION 1.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--- Declare options -----------------------------------------------------------
option(BUILD_DATA_MODEL "Run podio class generator yaml file" ON)

if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

include(GNUInstallDirs)

find_package(podio 0.15 REQUIRED)
include_directories(${podio_INCLUDE_DIR})

find_package(EDM4HEP 0.4.1 REQUIRED)

#include(cmake/root.cmake)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT 6 REQUIRED COMPONENTS Core RIO Tree)
include(${ROOT_USE_FILE})

PODIO_GENERATE_DATAMODEL(eicd eic_data.yaml headers sources
  UPSTREAM_EDM edm4hep:${EDM4HEP_DATA_DIR}/edm4hep.yaml
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
  )

PODIO_ADD_DATAMODEL_CORE_LIB(eicd "${headers}" "${sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)

PODIO_ADD_ROOT_IO_DICT(eicdDict eicd "${headers}" src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(eicd::eicdDict ALIAS eicdDict )

list(APPEND EICD_INSTALL_LIBS eicd eicdDict)

add_subdirectory(utils)

install(TARGETS ${EICD_INSTALL_LIBS}
  EXPORT EICDTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/eicd"
  COMPONENT dev)

install(FILES
  "${PROJECT_BINARY_DIR}/eicdDictDict.rootmap"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev)

install(FILES
  eic_data.yaml
  DESTINATION "${CMAKE_INSTALL_DATADIR}/eicd" COMPONENT dev)

install(FILES
  "${PROJECT_BINARY_DIR}/libeicdDict_rdict.pcm"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev)

# -------------------------
# install class documentation

cmake_path(RELATIVE_PATH CMAKE_CURRENT_BINARY_DIR
  OUTPUT_VARIABLE CMAKE_CURRENT_BINARY_RELATIVE_DIR)
configure_file(
  ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  @ONLY)

find_package(Doxygen COMPONENTS dot)
if(DOXYGEN_FOUND)
  add_custom_target(docs ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
else()
  message(STATUS "Doxygen not found; no documentation will be built.")
endif()

# -------------------------
# install library config
include(CMakePackageConfigHelpers)

# Install and export targets
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE EICD::
  DESTINATION lib/EICD
  )

set(TARGETS_INSTALL_PATH lib/EICD/EICDTargets.cmake)

configure_package_config_file(
  cmake/EICDConfig.cmake.in  
  ${CMAKE_CURRENT_BINARY_DIR}/EICDConfig.cmake
  INSTALL_DESTINATION lib/EICD
  PATH_VARS TARGETS_INSTALL_PATH
  )

write_basic_package_version_file("EICDConfigVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY SameMajorVersion
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/EICDConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/EICDConfigVersion.cmake
  DESTINATION lib/EICD
  )

