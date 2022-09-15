cmake_minimum_required(VERSION 3.11)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    cmake_policy(SET CMP0091 NEW)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

project(GenH5-root)

# also enable to search for hdf5 in devtools
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/hdf5)

add_subdirectory(src)

option(BUILD_UNITTESTS "Build Unittests" OFF)
if (BUILD_UNITTESTS)
    add_subdirectory(tests/unittests)
endif ()
