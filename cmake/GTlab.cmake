# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# This function set some variables that are common
# to the gtlab build system.
# It makes sure, that libraries are put into the root build folder,
# sets the debug postfix "-d" and sets the "binDebug" folder in
# Debug mode
#
# usage:
#   gtlab_standard_setup()
macro(gtlab_standard_setup)
    message("Setting up GTlab standards configuration")
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # store all dlls inside build directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})


    if(NOT DEFINED CMAKE_INSTALL_BINDIR)
        set(CMAKE_INSTALL_BINDIR "$<$<CONFIG:DEBUG>:binDebug>$<$<NOT:$<CONFIG:DEBUG>>:bin>")
    endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

    if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "-d")
    endif()

    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
endmacro()

# This function adds the packages of GTlab's devtools
# to the cmake prefix path. It adds the GTLAB_DEVTOOLS_DIR
# cache variable, to set the path to GTlab's devtools
#
# Usage:
#   enable_gtlab_devtools()
macro(enable_gtlab_devtools)
    # prefix 3rdparty stuff from devtools
    set(GTLAB_DEVTOOLS_DIR "" CACHE PATH "Path of gtlab devtools")

    if (EXISTS ${GTLAB_DEVTOOLS_DIR})
        message("GTlab DevTools enabled at " ${GTLAB_DEVTOOLS_DIR})

        set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
            ${GTLAB_DEVTOOLS_DIR}
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/BladeGenInterface
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/GoogleTest
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/hdf5
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/LibXML
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/minpack
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/NLopt
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/Qwt
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/SplineLib
        )
    endif()
	
endmacro()

# Function to add a gtlab module
#
# This makes sure to add a module id and deploy to the correct
# folders
#
# Usage:
#   add_gtlab_module(mymodule
#     SOURCES mod.cpp mod2.cpp mod.h
#     MODULE_ID "my mod"
#   )
function(add_gtlab_module GTLAB_ADD_MODULE_TARGET)
  cmake_parse_arguments(
    PARSE_ARGV 1 GTLAB_ADD_MODULE "" "MODULE_ID"
    "SOURCES")

  if (NOT DEFINED GTLAB_ADD_MODULE_MODULE_ID)
      message(FATAL_ERROR "In add_gtlab_module: Missing argument MODULE_ID for target ${GTLAB_ADD_MODULE_TARGET}")
  endif()

  add_library(${GTLAB_ADD_MODULE_TARGET} SHARED ${GTLAB_ADD_MODULE_SOURCES})

  # add module id
  target_compile_definitions(${GTLAB_ADD_MODULE_TARGET}
    PRIVATE -DGT_MODULE_ID="${GTLAB_ADD_MODULE_MODULE_ID}")

  # set rpath for linux
  if (UNIX)
      set_target_properties(${GTLAB_ADD_MODULE_TARGET}
          PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/.."
                     INSTALL_RPATH_USE_LINK_PATH FALSE)
  endif (UNIX)

  if (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/build)
    message(STATUS "Deploying into GTlab build dir")

    if (UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            LIBRARY DESTINATION build/modules
        )

    else(UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            RUNTIME DESTINATION build/modules
        )
    endif(UNIX)
  else ()
    if (UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            LIBRARY DESTINATION
            $<IF:$<CONFIG:Debug>,binDebug/modules,bin/modules>
        )
    else(UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            RUNTIME DESTINATION
            $<IF:$<CONFIG:Debug>,binDebug/modules,bin/modules>
        )
    endif(UNIX)
  endif ()
endfunction()

# ==============================================================================
# require_qt()
#
# Ensures the specified Qt components are available and mapped to versionless
# Qt:: targets, compatible with both Qt5 and Qt6.
#
# This function abstracts away differences between Qt5 and Qt6:
# - For Qt5, it calls `find_package(Qt5)` and creates alias targets like `Qt::Core`.
# - For Qt6, it sets QT_DEFAULT_MAJOR_VERSION and calls `find_package(Qt)` which provides `Qt::` targets directly.
#
# Parameters:
#   VERSION     Optional Qt major version (5 or 6). If omitted, uses QT_DEFAULT_MAJOR_VERSION or defaults to 5.
#   COMPONENTS  List of required Qt modules (e.g. Core, Widgets, Gui, etc.)
#
# Usage:
#   require_qt(
#       VERSION 5
#       COMPONENTS Core Widgets
#   )
#
# Notes:
#   - Safe to call from multiple CMake subprojects and CPM packages.
#   - Does not globally set or affect other Qt-related variables.
# ==============================================================================

function(require_qt)
    set(options)
    set(oneValueArgs VERSION)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(QT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Determine desired Qt version
    if(QT_VERSION)
        set(_qt_major_version ${QT_VERSION})
    elseif(DEFINED QT_DEFAULT_MAJOR_VERSION)
        set(_qt_major_version ${QT_DEFAULT_MAJOR_VERSION})
    else()
        set(_qt_major_version 5)
    endif()

    # Ensure COMPONENTS are specified
    if(NOT QT_COMPONENTS)
        message(FATAL_ERROR "require_qt() called without COMPONENTS")
    endif()

    # Find appropriate Qt version and set up targets
    if(_qt_major_version STREQUAL "5")
        find_package(Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
        foreach(comp IN LISTS QT_COMPONENTS)
            if(TARGET Qt5::${comp} AND NOT TARGET Qt::${comp})
                add_library(Qt::${comp} ALIAS Qt5::${comp})
            endif()
        endforeach()

    elseif(_qt_major_version STREQUAL "6")
        set(QT_DEFAULT_MAJOR_VERSION 6)  # Required before find_package(Qt)
        find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
        if(TARGET Qt6::${comp} AND NOT TARGET Qt::${comp})
            add_library(Qt::${comp} ALIAS Qt6::${comp})
        endif()
    else()
        message(FATAL_ERROR "require_qt(): Unsupported Qt version '${_qt_major_version}'")
    endif()

    set(QT_MAJOR_VERSION ${_qt_major_version} CACHE INTERNAL
        "Qt major version used to build")
endfunction()


