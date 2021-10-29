#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../../settings.pri )

TARGET_DIR_NAME = unittests
BUILD_DEST      = ../../build
MOC_BUILD_DEST  = ../../build

TARGET = H5UnitTest

QT += core

TEMPLATE = app
CONFIG += c++11 console silent

DEFINES += H5_BUILT_AS_DYNAMIC_LIB

INCLUDEPATH += .\

HEADERS += \
    testhelper.h
SOURCES += \
    h5/test_h5.cpp \
    h5/test_h5_abstractdataset.cpp \
    h5/test_h5_attribute.cpp \
    h5/test_h5_data.cpp \
    h5/test_h5_dataset.cpp \
    h5/test_h5_datasetproperties.cpp \
    h5/test_h5_dataspace.cpp \
    h5/test_h5_datatype.cpp \
    h5/test_h5_file.cpp \
    h5/test_h5_group.cpp \
    h5/test_h5_location.cpp \
    h5/test_h5_reference.cpp \
    main.cpp \
    testhelper.cpp

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}
    OBJECTS_DIR = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/obj
    MOC_DIR = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/moc
    RCC_DIR = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/rcc
    UI_DIR  = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/ui
} else {
    DESTDIR = $${BUILD_DEST}
    OBJECTS_DIR = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/obj
    MOC_DIR = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/moc
    RCC_DIR = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/rcc
    UI_DIR  = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/ui
}

### GTLAB HDF5
INCLUDEPATH += ../../src
LIBS        += -L../../lib/h5
DEPENDPATH  += ../../lib/h5

#### THIRD PARTY LIBRARIES
# Google Test
INCLUDEPATH += $${GOOGLE_TEST_PATH}/include
LIBS        += -L$${GOOGLE_TEST_PATH}/lib
DEPENDPATH  += $${GOOGLE_TEST_PATH}/lib

CONFIG(debug, debug|release) {
    # THIRD PARTY
    win32 {
        LIBS += -lhdf5 -lhdf5_cpp
        LIBS += -lgtestd
    }
    unix {
        LIBS += -lhdf5_debug -lhdf5_cpp_debug
        LIBS += -lgtest
    }
    # MODULES
    LIBS += -lGTlabH5-d
} else {
    # THIRD PARTY
    LIBS += -lhdf5 -lhdf5_cpp
    LIBS += -lgtest
    # MODULES
    LIBS += -lGTlabH5
}
