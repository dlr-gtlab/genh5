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
CONFIG += c++11

# always required
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
# disable deprecated symbols
DEFINES += H5_NO_DEPRECATED_SYMBOLS

win32 {
    DEFINES += GT_H5_DLL
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
    gt_h5.h \
    gt_h5abstractdata.h \
    gt_h5abtsractdataset.h \
    gt_h5attribute.h \
    gt_h5compounddata.h \
    gt_h5data.h \
    gt_h5dataset.h \
    gt_h5datasetproperties.h \
    gt_h5dataspace.h \
    gt_h5datatype.h \
    gt_h5file.h \
    gt_h5group.h \
    gt_h5leaf.h \
    gt_h5location.h \
    gt_h5node.h \
    gt_h5object.h \
    gt_h5_exports.h \
    gt_h5reference.h \
    gt_h5timelogger.h

SOURCES += \
    gt_h5abtsractdataset.cpp \
    gt_h5attribute.cpp \
    gt_h5dataset.cpp \
    gt_h5datasetproperties.cpp \
    gt_h5dataspace.cpp \
    gt_h5datatype.cpp \
    gt_h5file.cpp \
    gt_h5group.cpp \
    gt_h5leaf.cpp \
    gt_h5location.cpp \
    gt_h5node.cpp \
    gt_h5object.cpp \
    gt_h5reference.cpp

CONFIG(debug, debug|release) {
    LIBS += -lhdf5 -lhdf5_cpp
} else {
    LIBS += -lhdf5 -lhdf5_cpp
}
######################################################################

copyHeaders($$HEADERS)
copyToEnvironmentPath()

###################################################################### 
