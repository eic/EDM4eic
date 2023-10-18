# SPDX-License-Identifier: Apache-2.0

if(USE_EXTERNAL_CATCH2)
  find_package(Catch2 REQUIRED)
else()
  message(STATUS "Fetching local copy of Catch2 library for unit-tests...")

  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        6f21a3609cea360846a0ca93be55877cca14c86d
  )
  FetchContent_MakeAvailable(Catch2)
  set(CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras ${CMAKE_MODULE_PATH})
endif()

include(Catch)

add_executable(unittests
  test_kinematics.cpp
)
target_link_libraries(unittests edm4eic EDM4EIC::utils Catch2::Catch2 Catch2::Catch2WithMain)
catch_discover_tests(unittests)

add_test(NAME pyunittests COMMAND python -m unittest discover -s ${CMAKE_CURRENT_LIST_DIR})
set_property(TEST pyunittests
  PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=$<TARGET_FILE_DIR:edm4eic>:$<TARGET_FILE_DIR:podio::podio>:$<$<TARGET_EXISTS:edm4eicRDF>:$<TARGET_FILE_DIR:edm4eicRDF>:>$<TARGET_FILE_DIR:ROOT::Core>:$ENV{LD_LIBRARY_PATH}
  PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
  ROOT_INCLUDE_PATH=${PROJECT_SOURCE_DIR}/edm4eic:${PROJECT_SOURCE_DIR}/utils/include:$ENV{ROOT_INCLUDE_PATH}
  )
