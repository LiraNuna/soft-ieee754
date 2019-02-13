# Copyright (C) 2018 Jonathan Müller <jonathanmueller.dev@gmail.com>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

cmake_minimum_required(VERSION 3.11)
project(soft-ieee754)

# include_directories(${root_SOURCE_DIR}/include)
# Build tests if this is the main project
enable_testing()
add_subdirectory(tests)
