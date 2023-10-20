
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

# Get Qt5
find_package(Qt5 COMPONENTS Core REQUIRED)

set(EXAMPLE_SOURCE_FILES
  main.cpp
)

set(EXAMPLE_HEADER_FILES
)

add_executable(valgrind
  ${EXAMPLE_SOURCE_FILES}
  ${EXAMPLE_HEADER_FILES}
)

target_link_libraries(valgrind
    #Qt5::Core
    GenH5)
