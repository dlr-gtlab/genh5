project(GenH5-tests)

if (NOT TARGET GenH5)
    find_package(GenH5 CONFIG REQUIRED)
endif()


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT TARGET gtest)
    include(AddGoogleTest)
endif()

add_executable(H5UnitTest
    h5/test_h5.cpp
    h5/test_h5_abstractdataset.cpp
    h5/test_h5_attribute.cpp
    h5/test_h5_conversion.cpp
    h5/test_h5_data.cpp
    h5/test_h5_data0d.cpp
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

target_link_libraries(H5UnitTest PRIVATE GenH5 gtest)

