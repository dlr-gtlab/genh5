/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 07.04.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_VERSION_H
#define GTH5_VERSION_H

#define GTH5_TO_STR_HELPER(x) #x
#define GTH5_TO_STR(x) GTH5_TO_STR_HELPER(x)

#define GTH5_VERSION_MAJOR 1
#define GTH5_VERSION_MINOR 1
#define GTH5_VERSION_PATCH 0
#define GTH5_VERSION_ADDITIONAL ""
#define GTH5_VERSION ((GTH5_VERSION_MAJOR<<16)|\
                      (GTH5_VERSION_MINOR<<8)|\
                      (GTH5_VERSION_PATCH))

#define GTH5_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define GTH5_VERSION_STR \
    GTH5_TO_STR(GTH5_VERSION_MAJOR) "." \
    GTH5_TO_STR(GTH5_VERSION_MINOR) "." \
    GTH5_TO_STR(GTH5_VERSION_PATCH) \
    GTH5_VERSION_ADDITIONAL

#endif // GTH5_VERSION_H
