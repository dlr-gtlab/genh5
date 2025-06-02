# SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

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


