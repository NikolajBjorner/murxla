###
# Murxla: A Model-Based API Fuzzer for SMT solvers.
#
# This file is part of Murxla.
#
# Copyright (C) 2019-2022 by the authors listed in the AUTHORS file.
#
# See LICENSE for more information on using this software.
##
# Find Z3
# Z3_FOUND - found Z3 lib
# Z3_INCLUDE_DIR - the Z3 include directory
# Z3_LIBRARIES - Libraries needed to use Z3

find_path(Z3_INCLUDE_DIR NAMES z3++.h)
find_library(Z3_LIBRARIES NAMES z3)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Z3
  DEFAULT_MSG Z3_INCLUDE_DIR Z3_LIBRARIES)

mark_as_advanced(Z3_INCLUDE_DIR Z3_LIBRARIES)
