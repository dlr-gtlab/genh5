#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../settings.pri )

BUILD_DEST      = ../$${LIB_BUILD_DEST}
MOC_BUILD_DEST  = ../build

CONFIG(debug, debug|release) {
    TARGET = GTlabH5-d
} else {
    TARGET = GTlabH5
}

QT += core
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++14

# always required
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
# disable deprecated symbols
DEFINES += H5_NO_DEPRECATED_SYMBOLS

win32 {
    DEFINES += GTH5_DLL
}

CONFIG(debug, debug|release) {
    OBJECTS_DIR = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/obj
    MOC_DIR = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/moc
    RCC_DIR = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/rcc
    UI_DIR  = $${MOC_BUILD_DEST}/debug-src/$${TARGET_DIR_NAME}/ui
} else {
    OBJECTS_DIR = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/obj
    MOC_DIR = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/moc
    RCC_DIR = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/rcc
    UI_DIR  = $${MOC_BUILD_DEST}/release-src/$${TARGET_DIR_NAME}/ui
}

DESTDIR = $${BUILD_DEST}

INCLUDEPATH += .\

HEADERS += \
    gth5_.h \
    gth5_abstractdata.h \
    gth5_abtsractdataset.h \
    gth5_attribute.h \
    gth5_data.h \
    gth5_data_comp.h \
    gth5_data_simple.h \
    gth5_dataset.h \
    gth5_datasetproperties.h \
    gth5_dataspace.h \
    gth5_datatype.h \
    gth5_file.h \
    gth5_group.h \
    gth5_location.h \
    gth5_node.h \
    gth5_object.h \
    gth5_exports.h \
    gth5_reference.h \
    gth5_version.h

SOURCES += \
    gth5_abtsractdataset.cpp \
    gth5_attribute.cpp \
    gth5_dataset.cpp \
    gth5_datasetproperties.cpp \
    gth5_dataspace.cpp \
    gth5_datatype.cpp \
    gth5_file.cpp \
    gth5_group.cpp \
    gth5_location.cpp \
    gth5_node.cpp \
    gth5_object.cpp \
    gth5_reference.cpp

LIBS += -lhdf5 -lhdf5_cpp

######################################################################

copyHeaders($$HEADERS)
copyToEnvironmentPath()

###################################################################### 
