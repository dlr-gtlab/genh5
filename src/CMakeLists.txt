# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.8)
project(GenH5 VERSION 4.0.0-dev)

if (NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

# Compiler settings
if (NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(RequireQt)

# Get Qt5
require_qt(COMPONENTS Core REQUIRED)
find_package(HDF5 1.12.0 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(headers
    genh5.h
    genh5_abstractdataset.h
    genh5_attribute.h
    genh5_conversion.h
    genh5_conversion/bindings.h
    genh5_conversion/buffer.h
    genh5_conversion/defaults.h
    genh5_conversion/generic.h
    genh5_conversion/type.h
    genh5_data.h
    genh5_data/base.h
    genh5_data/comp.h
    genh5_data/comp0d.h
    genh5_data/buffer.h
    genh5_data/common.h
    genh5_data/common0d.h
    genh5_data/fixedstring0d.h
    genh5_dataset.h
    genh5_datasetcproperties.h
    genh5_dataspace.h
    genh5_datatype.h
    genh5_exception.h
    genh5_exports.h
    genh5_file.h
    genh5_group.h
    genh5_globals.h
    genh5_idcomponent.h
    genh5_location.h
    genh5_logging.h
    genh5_mpl.h
    genh5_node.h
    genh5_object.h
    genh5_optional.h
    genh5_private.h
    genh5_reference.h
    genh5_typedefs.h
    genh5_typetraits.h
    genh5_utils.h
    genh5_version.h
)

set(sources
    genh5_abstractdataset.cpp
    genh5_attribute.cpp
    genh5_dataset.cpp
    genh5_datasetcproperties.cpp
    genh5_dataspace.cpp
    genh5_datatype.cpp
    genh5_file.cpp
    genh5_group.cpp
    genh5_idcomponent.cpp
    genh5_location.cpp
    genh5_logging.cpp
    genh5_node.cpp
    genh5_object.cpp
    genh5_private.cpp
    genh5_reference.cpp
    genh5_utils.cpp
    genh5_version.cpp
)

# Tell CMake to run moc when necessary
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(GenH5 ${sources} ${headers})

if (${BUILD_SHARED_LIBS})
    target_compile_definitions(GenH5 PRIVATE GENH5_DLL)
else()
    target_compile_definitions(GenH5 PUBLIC GENH5_STATIC)
endif()

if (TARGET hdf5::hdf5 AND NOT TARGET hdf5-shared)
  add_library(hdf5-shared ALIAS hdf5::hdf5)
endif()

set_target_properties(GenH5 PROPERTIES VERSION 4.0.0 SOVERSION 4)

target_link_libraries(GenH5 PUBLIC Qt5::Core PRIVATE hdf5-shared)

target_include_directories(GenH5 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:include/h5>
)

# install library
install (TARGETS GenH5
         EXPORT GenH5Targets
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/h5
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/h5
)

# copy header files and keep directory structure
foreach ( file ${headers} )
    get_filename_component( dir ${file} DIRECTORY )
    install( FILES ${file} DESTINATION include/h5/${dir} )
endforeach()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/GenH5ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/GenH5Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/GenH5Config.cmake"
  @ONLY
)


install (EXPORT GenH5Targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenH5)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/GenH5ConfigVersion.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/GenH5Config.cmake
         DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenH5)
