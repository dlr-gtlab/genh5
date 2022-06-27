#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include($${PWD}/../settings.pri)

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

# H5Cpp specific
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
# disable deprecated symbols
DEFINES += H5_NO_DEPRECATED_SYMBOLS

win32 {
    DEFINES += GTH5_DLL
}

CONFIG(debug, debug|release){
    MOC_BUILD_DEST = $${MOC_BUILD_DEST}/debug_$${TARGET_DIR_NAME}
} else {
    MOC_BUILD_DEST = $${MOC_BUILD_DEST}/release_$${TARGET_DIR_NAME}
}

DESTDIR = $${BUILD_DEST}
OBJECTS_DIR = $${MOC_BUILD_DEST}/obj
MOC_DIR = $${MOC_BUILD_DEST}/moc
RCC_DIR = $${MOC_BUILD_DEST}/rcc
UI_DIR  = $${MOC_BUILD_DEST}/ui

DESTDIR = $${BUILD_DEST}

INCLUDEPATH += .\

HEADERS += \
    gth5.h \
    gth5_abtsractdataset.h \
    gth5_attribute.h \
    gth5_conversion.h \
    gth5_conversion/bindings.h \
    gth5_conversion/buffer.h \
    gth5_conversion/defaults.h \
    gth5_conversion/generic.h \
    gth5_conversion/type.h \
    gth5_data.h \
    gth5_data/base.h \
    gth5_data/comp.h \
    gth5_data/simple.h \
    gth5_dataset.h \
    gth5_datasetcproperties.h \
    gth5_dataspace.h \
    gth5_datatype.h \
    gth5_exception.h \
    gth5_file.h \
    gth5_group.h \
    gth5_location.h \
    gth5_mpl.h \
    gth5_node.h \
    gth5_object.h \
    gth5_exports.h \
    gth5_optional.h \
    gth5_reference.h \
    gth5_typedefs.h \
    gth5_typetraits.h \
    gth5_utils.h \
    gth5_version.h

SOURCES += \
    gth5_abtsractdataset.cpp \
    gth5_attribute.cpp \
    gth5_dataset.cpp \
    gth5_datasetcproperties.cpp \
    gth5_dataspace.cpp \
    gth5_datatype.cpp \
    gth5_file.cpp \
    gth5_group.cpp \
    gth5_location.cpp \
    gth5_node.cpp \
    gth5_reference.cpp \
    gth5_utils.cpp

CONFIG(debug, debug|release) {
    win32 {
        LIBS += -lhdf5_D -lhdf5_cpp_D
    }
    unix {
        LIBS += -lhdf5 -lhdf5_cpp
    }
} else {
    LIBS += -lhdf5 -lhdf5_cpp
}

######################################################################

KEEP_INCLUDEPATHS += \
    gth5_conversion \
    gth5_data

PRIVATE_INCLUDEPATHS += \

copyHeaders($$HEADERS, $$KEEP_INCLUDEPATHS, $$PRIVATE_INCLUDEPATHS)
copyToEnvironmentPath()

###################################################################### 
