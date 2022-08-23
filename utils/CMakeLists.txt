find_package(ROOT REQUIRED COMPONENTS GenVector MathCore)

add_library(eicd_utils SHARED
  src/utils.cxx
  )

target_compile_features(eicd_utils
    PUBLIC cxx_auto_type
    PUBLIC cxx_trailing_return_types
    PUBLIC cxx_std_17
    PRIVATE cxx_variadic_templates
    )

target_compile_options(eicd_utils PRIVATE 
  -Wno-extra
  -Wno-ignored-qualifiers
  -Wno-overloaded-virtual
  -Wno-shadow)

target_link_libraries(eicd_utils 
  PUBLIC eicd
  PUBLIC EDM4HEP::edm4hep
  PUBLIC ROOT::GenVector ROOT::MathCore)

target_include_directories(eicd_utils 
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
  )

install(FILES 
  include/eicd/analysis_utils.h
  include/eicd/vector_utils.h
  include/eicd/vector_utils_legacy.h
  DESTINATION include/eicd
  )

install(TARGETS eicd_utils
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  )
