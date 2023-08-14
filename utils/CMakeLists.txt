# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2022 Whitney Armstrong, Sylvester Joosten, Wouter Deconinck

find_package(ROOT REQUIRED COMPONENTS GenVector MathCore)
find_package(CLI11 CONFIG)

add_library(utils INTERFACE)
add_library(EDM4EIC::utils ALIAS utils)

target_link_libraries(utils
  INTERFACE edm4eic
  INTERFACE EDM4HEP::edm4hep
  INTERFACE ROOT::GenVector ROOT::MathCore
  )

target_include_directories(utils
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  INTERFACE $<INSTALL_INTERFACE:include>
  )

install(FILES
  include/edm4eic/utils/analysis_utils.h
  include/edm4eic/utils/unit_system.h
  include/edm4eic/utils/vector_utils.h
  include/edm4eic/utils/vector_utils_legacy.h
  DESTINATION include/edm4eic/utils
  )

install(TARGETS utils
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  )

if(CLI11_FOUND)

  add_executable(edm4eic_merge src/merge.cpp)

  target_compile_features(edm4eic_merge
    PUBLIC cxx_auto_type
    PUBLIC cxx_trailing_return_types
    PUBLIC cxx_std_17
    PRIVATE cxx_variadic_templates
    )

  target_compile_options(edm4eic_merge PRIVATE
    -Wno-extra
    -Wno-ignored-qualifiers
    -Wno-overloaded-virtual
    -Wno-shadow
    )

  target_include_directories(edm4eic_merge
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
    )

  target_link_libraries(edm4eic_merge
    PUBLIC edm4eic
    PUBLIC EDM4HEP::edm4hep
    PUBLIC podio::podio podio::podioRootIO
    PUBLIC ROOT::GenVector ROOT::MathCore)

  install(TARGETS edm4eic_merge
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

endif()
