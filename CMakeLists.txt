cmake_minimum_required(VERSION 3.8)
project(EICD 
  VERSION 0.5.1
  LANGUAGES CXX)

#--- Declare options -----------------------------------------------------------
option(CREATE_DOC "Whether or not to create doxygen doc target.")
option(BUILD_DATA_MODEL "Run podio class generator yaml file" OFF)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")


if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

set(DEF_INSTALL_CMAKE_DIR cmake)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

if(BUILD_DATA_MODEL)
  execute_process(COMMAND python $ENV{PODIO}/python/podio_class_generator.py ${CMAKE_SOURCE_DIR}/eic_data.yaml ${PROJECT_BINARY_DIR} eicd )
  #execute_process(COMMAND python $ENV{PODIO}/python/podio_class_generator.py ${CMAKE_SOURCE_DIR}/dd4hep.yaml ${PROJECT_BINARY_DIR}/dd4pod dd4hep/podio )
endif(BUILD_DATA_MODEL)

# Make relative paths absolute (needed later on)
#foreach(p LIB BIN INCLUDE CMAKE)
#  set(var INSTALL_${p}_DIR)
#  if(NOT IS_ABSOLUTE "${${var}}")
#    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
#  endif()
#endforeach()

# Set up C++14
##set(CPP_STANDARD_FLAGS "-std=c++17")
#if (${APPLE})
#    set(CPP_STANDARD_FLAGS "-std=c++14\ -stdlib=libc++")
#endif()
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL ${CPP_STANDARD_FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")

# Make sure we find the Find*.cmake functions distributed with this package
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(podio REQUIRED)
message("Podio libraries: ${podio_LIBRARIES}")
# Make sure the library is found.
# Not the case if LD_LIBRARY_PATH is wrong
#find_library(PODIOLIB podio PATHS $ENV{PODIO}/lib)
#if (NOT PODIOLIB)
#    message(FATAL_ERROR "libpodio.so(dylib) cannot be found dynamically. Make sure you have sourced PODIO init*.sh file to set up your environment to use PODIO")
#endif()
#link_directories(${podio_LIBRARY_DIR})

#--- Declare ROOT dependency ---------------------------------------------------
include(cmake/root.cmake)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(ROOT REQUIRED COMPONENTS RIO Tree Physics)
include(${ROOT_USE_FILE})
## extra root settings
#find_package(dag REQUIRED HINTS $ENV{FCCDAG})
#if(${DAG_FOUND})
#  message(STATUS "Found dag: ${DAG_DIR}")
#  include_directories(
#          ${DAG_INCLUDE_DIRS}
#  )
#endif(${DAG_FOUND})

#link_directories(${ROOT_LIBRARY_DIR})
#add_definitions(-Wpadded)
#--temporary fix of inconsistency in ROOT CMake macros
#set(ROOT_genreflex_cmd ${ROOT_genreflex_CMD})

#add_definitions(-Wno-unused-variable -Wno-unused-parameter)

#configure_file(${CMAKE_SOURCE_DIR}/cmake/Datamodel.cmake
#  "${CMAKE_SOURCE_DIR}/datamodel/CMakeLists.txt" @ONLY)
# Add sub-directories
#add_subdirectory(datamodel)
#add_subdirectory(utilities)
#add_subdirectory(examples)
#add_subdirectory(tests)

file(GLOB sources ${CMAKE_BINARY_DIR}/src/*.cc)
file(GLOB headers ${CMAKE_BINARY_DIR}/eicd/*.h podio/PythonEventStore.h)

#REFLEX_GENERATE_DICTIONARY(EICD ${headers} SELECTION ${CMAKE_BINARY_DIR}/src/selection.xml )
#ROOT_GENERATE_DICTIONARY(EICDDict ${headers} LINKDEF ${CMAKE_BINARY_DIR}/LinkDef.h MODULE EICD)# OPTIONS -p)
#ADD_CUSTOM_TARGET(EICD_ROOTDICTS DEPENDS ${sources} ${headers} EICDDict.cxx)

add_library(EICD SHARED ${sources} ) #EICDDict.cxx)
target_link_libraries(EICD 
  PUBLIC podio::podio
  PUBLIC ROOT::Core
  PUBLIC ROOT::GenVector
  ${podio_LIBRARIES}
  )
target_include_directories(EICD 
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/eicd>
  PUBLIC 
  $<INSTALL_INTERFACE:include/eicd>
  )
#add_dependencies(EICD EICD_ROOTDICTS)

#add_library(EICDDict SHARED EICD.cxx)
#target_link_libraries(EICDDict)

set_target_properties(EICD PROPERTIES
  PUBLIC_HEADER "${headers}")

install(TARGETS EICD
  # IMPORTANT: Add the datamodel library to the "export-set"
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/eicd"
  COMPONENT dev)

#install(FILES
#  "${PROJECT_BINARY_DIR}/libEICD.rootmap"
#  DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev)
#install(FILES
#  "${PROJECT_BINARY_DIR}/libEICD_rdict.pcm"
#  DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev)


## The interesting stuff goes here
## ===============================
#
## Add all targets to the build-tree export set
#export(TARGETS datamodel utilities EICD-write EICD-read EICD-simplewrite
#  FILE "${PROJECT_BINARY_DIR}/EICDTargets.cmake")
#
## Export the package for use from the build-tree
## (this registers the build-tree with a global CMake-registry)
#export(PACKAGE EICD)
#
## Create the EICDConfig.cmake and EICDConfigVersion files
#file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
#   "${INSTALL_INCLUDE_DIR}")
## ... for the build tree
#set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")

#configure_file(cmake/EICDConfig.cmake.in
#  "${PROJECT_BINARY_DIR}/EICDConfig.cmake" @ONLY)
#
#configure_file(cmake/EICDConfigVersion.cmake.in
#  "${PROJECT_BINARY_DIR}/EICDConfigVersion.cmake" @ONLY)
#
## Install the EICDConfig.cmake and EICDConfigVersion.cmake
#install(FILES
#  "${PROJECT_BINARY_DIR}/EICDConfig.cmake"
#  "${PROJECT_BINARY_DIR}/EICDConfigVersion.cmake"
#  DESTINATION lib/EICD)
##
### Install the export set for use with the install-tree


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

