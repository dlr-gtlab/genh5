# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

option(GENH5_HDF5_STATIC "Link HDF5 statically" OFF)

find_package(hdf5 CONFIG REQUIRED)

# hdf5::hdf5 might already be created by conan or other package managers
if (NOT TARGET hdf5::hdf5)

    # lets create an hdf5::hdf5 alias target based on the static/dynamic option
    if (GENH5_HDF5_STATIC)
        if (TARGET hdf5-static)
            if (HDF5_ENABLE_Z_LIB_SUPPORT)
                # One-time status message
                if (NOT DEFINED GENH5_SEARCH_ZLIB_REPORTED)
                    message("Found HDF5 with zlib support, searching for ZLIB...")
                    set(GENH5_SEARCH_ZLIB_REPORTED TRUE CACHE INTERNAL
                        "Whether zlib search has been reported")
                endif()

                find_package(ZLIB REQUIRED)
            endif()

            add_library(hdf5::hdf5 ALIAS hdf5-static)
        else()
            message(FATAL_ERROR "GENH5_HDF5_STATIC is ON but hdf5-static target not found")
        endif()
    else()
        if (TARGET hdf5-shared)
            add_library(hdf5::hdf5  ALIAS hdf5-shared)
        else()
            message(FATAL_ERROR "GENH5_HDF5_STATIC is OFF but hdf5-shared target not found")
        endif()
    endif()
endif()