#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include(settings.pri)

TEMPLATE = subdirs
CONFIG += c++14

#### Interface ####
contains(BUILD_HDF5, true) {
    message("BUILD_HDF5 = true")
    SUBDIRS += src
}

#### UNIT TESTS ####
contains(BUILD_UNITTESTS, true) {
    message("BUILD_UNITTESTS = true")
    SUBDIRS += unittests
    unittests.subdir = tests/unittests
    unittests.depends = src
}

#### UNIT TESTS ####
contains(BUILD_EXAMPLES, true) {
    message("BUILD_EXAMPLES = true")
    SUBDIRS += examples
    examples.depends = src
}

######################################################################


