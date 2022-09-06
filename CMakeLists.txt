cmake_minimum_required(VERSION 3.12)
project(EDM4EIC
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
# The target for the data model library is defined in podio/cmake/podioMacros.cmake
# Any changes to that target name upstream may require changes to the target used here 
target_include_directories(edm4eic PUBLIC ${EDM4HEP_INCLUDE_DIR})
# It's our responsibility to link to upstream datamodel
target_link_libraries(edm4eic PRIVATE EDM4HEP::edm4hep)

PODIO_ADD_ROOT_IO_DICT(edm4eicDict edm4eic "${headers}" src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(edm4eic::edm4hepDict ALIAS edm4eicDict )

list(APPEND EDM4EIC_INSTALL_LIBS edm4eic edm4eicDict)

add_subdirectory(utils)

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
  VERSION ${VERSION}
  COMPATIBILITY SameMajorVersion
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/${PROJECT_NAME}
  )
