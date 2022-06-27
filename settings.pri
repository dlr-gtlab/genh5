#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

#### LOCAL SETTINGS
TARGET_DIR_NAME   = h5
LIB_BUILD_DEST    = lib/$${TARGET_DIR_NAME}

include(local_settings.pri)
include(deployment.pri)

#### THIRD PARTY LIBRARIES
#### HDF5
CONFIG(debug, debug|release) {
    message(DEBUG)
    INCLUDEPATH += $${HDF5_PATH}/include
    LIBS        += -L$${HDF5_PATH}/libDebug
    DEPENDPATH  += $${HDF5_PATH}/libDebug
} else {
    message(RELEASE)
    INCLUDEPATH += $${HDF5_PATH}/include
    LIBS        += -L$${HDF5_PATH}/lib
    DEPENDPATH  += $${HDF5_PATH}/lib
}

######################################################################
