# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.12)
project(PackageTest CXX)

find_package(GenH5 REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example GenH5)
