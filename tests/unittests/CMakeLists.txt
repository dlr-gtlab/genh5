# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

project(GenH5-tests)

if (NOT TARGET GenH5)
    find_package(GenH5 CONFIG REQUIRED)
endif()

if (NOT TARGET hdf5::hdf5_cpp)
    find_package(HDF5 CONFIG REQUIRED)
endif()

# make conan and normal config files work
if (TARGET hdf5::hdf5_cpp AND NOT TARGET hdf5_cpp-shared)
  add_library(hdf5_cpp-shared ALIAS hdf5::hdf5_cpp)
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT TARGET gtest)
    include(AddGoogleTest)
endif()

add_executable(H5UnitTest
    h5/test_h5.cpp
    h5/test_h5_abstractdataset.cpp
    h5/test_h5_attribute.cpp
    h5/test_h5_bugs.cpp
    h5/test_h5_conversion.cpp
    h5/test_h5_data.cpp
    h5/test_h5_data0d.cpp
    h5/test_h5_data_fixedstring0d.cpp
    h5/test_h5_dataset.cpp
    h5/test_h5_datasetcproperties.cpp
    h5/test_h5_dataspace.cpp
    h5/test_h5_datatype.cpp
    h5/test_h5_exception.cpp
    h5/test_h5_file.cpp
    h5/test_h5_group.cpp
    h5/test_h5_iteration.cpp
    h5/test_h5_location.cpp
    h5/test_h5_node.cpp
    h5/test_h5_reference.cpp
    h5/test_h5_utils.cpp
    h5/test.cpp
    main.cpp
    testhelper.cpp
    testhelper.h
)

target_include_directories(H5UnitTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(H5UnitTest PRIVATE GenH5 gtest hdf5_cpp-shared)

include(GoogleTest)
gtest_discover_tests(H5UnitTest TEST_PREFIX "GenH5." DISCOVERY_MODE PRE_TEST)
