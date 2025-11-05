# SPDX-License-Identifier: Apache-2.0

IF(NOT BUILD_TESTING)
  RETURN()
ENDIF()

function(set_test_env _testname)
  set_property(TEST ${_testname} APPEND PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=$<TARGET_FILE_DIR:edm4eic>:$<TARGET_FILE_DIR:EDM4HEP::edm4hep>:$<TARGET_FILE_DIR:podio::podio>:$<$<TARGET_EXISTS:EDM4HEP::edm4hepRDF>:$<TARGET_FILE_DIR:EDM4HEP::edm4hepRDF>:>$<TARGET_FILE_DIR:ROOT::Core>:$ENV{LD_LIBRARY_PATH}
      ROOT_INCLUDE_PATH=${PROJECT_SOURCE_DIR}/utils/include:$ENV{ROOT_INCLUDE_PATH}
  )
  set_tests_properties(${_testname} PROPERTIES
    FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;error;Failed"
    )
endfunction()

add_executable(write_events write_events.cc)
target_include_directories(write_events PUBLIC ${PROJECT_SOURCE_DIR}/edm4hep )
target_link_libraries(write_events edm4eic EDM4HEP::edm4hep podio::podioRootIO)
add_test(NAME write_events COMMAND write_events)
set_test_env(write_events)

add_executable(read_events read_events.cc)
target_include_directories(read_events PUBLIC ${PROJECT_SOURCE_DIR}/edm4hep )
target_link_libraries(read_events edm4eic EDM4HEP::edm4hep podio::podioRootIO)
add_test(NAME read_events COMMAND read_events)
  set_property(TEST read_events PROPERTY
    DEPENDS write_events
    )
set_test_env(read_events)

IF(TARGET ROOT::ROOTDataFrame)
  add_executable(test_rdf test_rdf.cc)
  target_include_directories(test_rdf PUBLIC ${PROJECT_SOURCE_DIR}/edm4hep ${PROJECT_SOURCE_DIR}/dataframe )
  target_link_libraries(test_rdf edm4eic EDM4HEP::edm4hepRDF ROOT::ROOTDataFrame podio::podioRootIO)
  add_test(NAME test_rdf COMMAND test_rdf)
  set_test_env(test_rdf)
  set_property(TEST test_rdf PROPERTY
    DEPENDS write_events
    )

  add_test(NAME py_test_rdf COMMAND python ${CMAKE_CURRENT_LIST_DIR}/test_rdf.py)
  set_test_env(py_test_rdf)
  get_property(ENVIRONMENT TEST py_test_rdf PROPERTY ENVIRONMENT)
  set_property(TEST py_test_rdf PROPERTY ENVIRONMENT
      ${ENVIRONMENT}
      PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
  )
  set_tests_properties(py_test_rdf PROPERTIES DEPENDS write_events)
endif()

if (nlohmann_json_FOUND)
  add_test(NAME convert_events COMMAND edm4hep2json edm4eic_events.root)
  set_property(TEST convert_events PROPERTY DEPENDS write_events)
  set_test_env(convert_events)
endif()
