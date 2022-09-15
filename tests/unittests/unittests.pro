#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include($${PWD}/../../settings.pri)

TARGET_DIR_NAME = unittests
BUILD_DEST      = ../../build
MOC_BUILD_DEST  = $${BUILD_DEST}

TARGET = H5UnitTest

QT += core

TEMPLATE = app
CONFIG += console

# H5Cpp specific
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
DEFINES += H5_NO_DEPRECATED_SYMBOLS
# GenH5 specific
#DEFINES += GENH5_NO_DEPRECATED_SYMBOLS
#DEFINES += GENH5_NO_BUFFER_PRE_RESERVING

CONFIG(debug, debug|release){
    MOC_BUILD_DEST = $${MOC_BUILD_DEST}/debug_$${TARGET_DIR_NAME}
} else {
    MOC_BUILD_DEST = $${MOC_BUILD_DEST}/release_$${TARGET_DIR_NAME}
}

OBJECTS_DIR = $${MOC_BUILD_DEST}/obj
MOC_DIR = $${MOC_BUILD_DEST}/moc
RCC_DIR = $${MOC_BUILD_DEST}/rcc
UI_DIR  = $${MOC_BUILD_DEST}/ui

DESTDIR = $${BUILD_DEST}

INCLUDEPATH += .\

HEADERS += \
    testhelper.h

SOURCES += \
    h5/test_h5.cpp \
    h5/test_h5_abstractdataset.cpp \
    h5/test_h5_attribute.cpp \
    h5/test_h5_conversion.cpp \
    h5/test_h5_data.cpp \
    h5/test_h5_data0d.cpp \
    h5/test_h5_dataset.cpp \
    h5/test_h5_datasetcproperties.cpp \
    h5/test_h5_dataspace.cpp \
    h5/test_h5_datatype.cpp \
    h5/test_h5_exception.cpp \
    h5/test_h5_file.cpp \
    h5/test_h5_group.cpp \
    h5/test_h5_iteration.cpp \
    h5/test_h5_location.cpp \
    h5/test_h5_node.cpp \
    h5/test_h5_optional.cpp \
    h5/test_h5_reference.cpp \
    h5/test_h5_utils.cpp \
    h5/test.cpp \
    main.cpp \
    testhelper.cpp

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
    win32 {
        LIBS += -lhdf5_D -lhdf5_cpp_D
    }
    unix {
        LIBS += -lhdf5 -lhdf5_cpp
    }
    LIBS += -lGenH5-d
    # THIRD PARTY
    win32 {
        LIBS += -lgtestd
    }
    unix {
        LIBS += -lgtest
    }
} else {
    LIBS += -lhdf5 -lhdf5_cpp
    LIBS += -lGenH5
    # THIRD PARTY
    LIBS += -lgtest
}

unix:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}
