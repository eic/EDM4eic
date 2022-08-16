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


find_package(podio 0.15 REQUIRED)
include_directories(${podio_INCLUDE_DIR})

find_package(EDM4HEP 0.4.1 REQUIRED)

#include(cmake/root.cmake)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
include(${ROOT_USE_FILE})

PODIO_GENERATE_DATAMODEL(eicd eic_data.yaml headers sources 
  UPSTREAM_EDM edm4hep:${EDM4HEP_DATA_DIR}/edm4hep.yaml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(eicd SHARED
  ${sources}
  )

target_link_libraries(eicd
  PUBLIC EDM4HEP::edm4hep
  PUBLIC podio::podio
  PUBLIC ROOT::Core
  )
target_include_directories(eicd
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>)

target_compile_features(eicd
  PUBLIC cxx_auto_type
  PUBLIC cxx_trailing_return_types
  PRIVATE cxx_variadic_templates
  PRIVATE cxx_std_17)

PODIO_GENERATE_DICTIONARY(eicd ${headers} 
  SELECTION ${CMAKE_CURRENT_BINARY_DIR}/src/selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}eicd${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
set_target_properties(eicd-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_sources(eicd PRIVATE eicd.cxx)

add_subdirectory(utils)


install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/eicd
  DESTINATION include
  FILES_MATCHING PATTERN *.h
  )

install(TARGETS eicd
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  )

install(FILES
  "${PROJECT_BINARY_DIR}/eicdDict.rootmap"
  "${PROJECT_BINARY_DIR}/libeicd_rdict.pcm"
  DESTINATION lib)

add_custom_target( doc_doxygen #ALL 
  COMMAND doxygen ${CMAKE_SOURCE_DIR}/docs/Doxyfile
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM )



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

