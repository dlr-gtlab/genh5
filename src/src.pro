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
    TARGET = GenH5-d
} else {
    TARGET = GenH5
}

QT += core

TEMPLATE = lib
CONFIG += plugin

# H5Cpp specific
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
# disable deprecated symbols
DEFINES += H5_NO_DEPRECATED_SYMBOLS
# GenH5 specific
#DEFINES += GENH5_NO_DEPRECATED_SYMBOLS
#DEFINES += GENH5_NO_BUFFER_RESERVING
#DEFINES += GENH5_NO_STATIC_BUFFER
#DEFINES += GENH5_NO_DATA_AUTORESIZE

win32 {
    DEFINES += GENH5_DLL
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
    genh5.h \
    genh5_abstractdataset.h \
    genh5_attribute.h \
    genh5_conversion.h \
    genh5_conversion/bindings.h \
    genh5_conversion/buffer.h \
    genh5_conversion/defaults.h \
    genh5_conversion/generic.h \
    genh5_conversion/type.h \
    genh5_data.h \
    genh5_data/base.h \
    genh5_data/buffer.h \
    genh5_data/common.h \
    genh5_data/common0d.h \
    genh5_data/comp.h \
    genh5_data/comp0d.h \
    genh5_data/fixedstring0d.h \
    genh5_dataset.h \
    genh5_datasetcproperties.h \
    genh5_dataspace.h \
    genh5_datatype.h \
    genh5_exception.h \
    genh5_file.h \
    genh5_group.h \
    genh5_location.h \
    genh5_mpl.h \
    genh5_node.h \
    genh5_object.h \
    genh5_exports.h \
    genh5_optional.h \
    genh5_reference.h \
    genh5_typedefs.h \
    genh5_typetraits.h \
    genh5_utils.h \
    genh5_version.h

SOURCES += \
    genh5_abstractdataset.cpp \
    genh5_attribute.cpp \
    genh5_dataset.cpp \
    genh5_datasetcproperties.cpp \
    genh5_dataspace.cpp \
    genh5_datatype.cpp \
    genh5_file.cpp \
    genh5_group.cpp \
    genh5_location.cpp \
    genh5_node.cpp \
    genh5_object.cpp \
    genh5_reference.cpp \
    genh5_utils.cpp \
    genh5_version.cpp

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
    genh5_conversion \
    genh5_data

PRIVATE_INCLUDEPATHS += \

copyHeaders($$HEADERS, $$KEEP_INCLUDEPATHS, $$PRIVATE_INCLUDEPATHS)
copyToEnvironmentPath()

###################################################################### 
