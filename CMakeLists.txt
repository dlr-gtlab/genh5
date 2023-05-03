cmake_minimum_required(VERSION 3.11)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    cmake_policy(SET CMP0091 NEW)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

project(GenH5-root)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GTlab)

gtlab_standard_setup()
enable_gtlab_devtools()

# also enable to search for hdf5 in devtools
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/hdf5)


option(BUILD_UNITTESTS "Build Unittests" OFF)
option(BUILD_WITH_COVERAGE "Build with code coverage (linux only)" OFF)

if (BUILD_WITH_COVERAGE)
    set(CODE_COVERAGE_VERBOSE ON)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif(BUILD_WITH_COVERAGE)

add_subdirectory(src)


if (BUILD_UNITTESTS)
    enable_testing()
    add_subdirectory(tests/unittests)
endif ()


if (BUILD_UNITTESTS AND BUILD_WITH_COVERAGE)
    setup_target_for_coverage_lcov(
            NAME test-coverage
            EXECUTABLE H5UnitTest
            EXCLUDE "tests/*" "build*" "/usr/*" "/opt/*"
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif()