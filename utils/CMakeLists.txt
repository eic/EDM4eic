find_package(ROOT REQUIRED COMPONENTS GenVector MathCore)
find_package(CLI11 CONFIG)

add_library(edm4eic_utils SHARED
  src/utils.cxx
  )

target_compile_features(edm4eic_utils
    PUBLIC cxx_auto_type
    PUBLIC cxx_trailing_return_types
    PUBLIC cxx_std_17
    PRIVATE cxx_variadic_templates
    )

target_compile_options(edm4eic_utils PRIVATE
  -Wno-extra
  -Wno-ignored-qualifiers
  -Wno-overloaded-virtual
  -Wno-shadow)

target_link_libraries(edm4eic_utils
  PUBLIC edm4eic
  PUBLIC EDM4HEP::edm4hep
  PUBLIC ROOT::GenVector ROOT::MathCore)

target_include_directories(edm4eic_utils
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
  )

install(FILES
  include/edm4eic/analysis_utils.h
  include/edm4eic/vector_utils.h
  include/edm4eic/vector_utils_legacy.h
  DESTINATION include/edm4eic
  )

install(TARGETS edm4eic_utils
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
