###
# Murxla: A Model-Based API Fuzzer for SMT solvers.
#
# This file is part of Murxla.
#
# Copyright (C) 2019-2022 by the authors listed in the AUTHORS file.
#
# See LICENSE for more information on using this software.
##
set(test_util_src_files
  ${PROJECT_SOURCE_DIR}/src/util.cpp
  test_util.cpp
)
add_executable (testutil ${test_util_src_files})
target_include_directories(testutil PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(testutil gtest_main)
set_target_properties(testutil PROPERTIES OUTPUT_NAME testutil)
add_test(util ${CMAKE_BINARY_DIR}/bin/testutil)

# Check if Z3 is available by checking if the variables are set
if(Z3_INCLUDE_DIR AND Z3_LIBRARIES)
  message(STATUS "Building Z3 tests")
  set(test_z3_src_files
    test_z3.cpp
  )
  add_executable(testz3 ${test_z3_src_files})
  target_include_directories(testz3 PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${Z3_INCLUDE_DIR}
  )
  target_link_libraries(testz3 gtest_main ${Z3_LIBRARIES})
  target_compile_definitions(testz3 PUBLIC MURXLA_USE_Z3)
  set_target_properties(testz3 PROPERTIES OUTPUT_NAME testz3)
  add_test(z3 ${CMAKE_BINARY_DIR}/bin/testz3)
else()
  message(STATUS "Z3 not found, skipping Z3 tests")
endif()
