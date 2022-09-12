cmake_minimum_required(VERSION 3.8)
project(GenH5 VERSION 2.0.0)


if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)


if(NOT DEFINED CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "$<$<CONFIG:DEBUG>:binDebug>$<$<NOT:$<CONFIG:DEBUG>>:bin>")
endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Compiler settings
if (NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)

# Get Qt5
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(HDF5 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(headers
    genh5.h
    genh5_abtsractdataset.h
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
    genh5_data/common.h
    genh5_data/common0d.h
    genh5_dataset.h
    genh5_datasetcproperties.h
    genh5_dataspace.h
    genh5_datatype.h
    genh5_exception.h
    genh5_file.h
    genh5_group.h
    genh5_location.h
    genh5_mpl.h
    genh5_node.h
    genh5_object.h
    genh5_exports.h
    genh5_optional.h
    genh5_reference.h
    genh5_typedefs.h
    genh5_typetraits.h
    genh5_utils.h
    genh5_version.h
)

set(sources
    genh5_abtsractdataset.cpp
    genh5_attribute.cpp
    genh5_dataset.cpp
    genh5_datasetcproperties.cpp
    genh5_dataspace.cpp
    genh5_datatype.cpp
    genh5_file.cpp
    genh5_group.cpp
    genh5_location.cpp
    genh5_node.cpp
    genh5_object.cpp
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

if (TARGET hdf5::hdf5_cpp AND NOT TARGET hdf5_cpp-shared)
  add_library(hdf5_cpp-shared ALIAS hdf5::hdf5_cpp)
endif()

target_link_libraries(GenH5 PUBLIC Qt5::Core hdf5_cpp-shared)

target_include_directories(GenH5 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:include/genh5>
)

# install library
install (TARGETS GenH5
         EXPORT GenH5-targets
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# copy header files and keep directory structure
foreach ( file ${headers} )
    get_filename_component( dir ${file} DIRECTORY )
    install( FILES ${file} DESTINATION include/genh5/${dir} )
endforeach()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/GenH5-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

file(GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/GenH5-config.cmake"
  INPUT  "${PROJECT_SOURCE_DIR}/cmake/GenH5-config.cmake.in"
)


install (EXPORT GenH5-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenH5)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/GenH5-config-version.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/GenH5-config.cmake
         DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenH5)