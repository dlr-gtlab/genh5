# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

LIB_DIR=./lib/$TARGETDIRNAME
QT_DIR=/opt/Qt/5.12.5/gcc_64

export LIBRARY_PATH=$LIB_DIR:$LIBRARY_PATH
export LIBRARY_PATH=$QT_DIR:$LIBRARY_PATH
export LIBRARY_PATH=$QT_DIR/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/minpack/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/hdf5/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/hdf5/libDebug:$LIBRARY_PATH
export LD_LIBRARY_PATH=$LIBRARY_PATH
./build/$UNITTESTSNAME --gtest_output=xml:unittests.xml
