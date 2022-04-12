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

#define GTH5_VERSION 0x10003
#define GTH5_VERSION_MAJOR 1
#define GTH5_VERSION_MINOR 0
#define GTH5_VERSION_PATCH 3
#define GTH5_VERSION_ADDITIONAL "-alpha"

#define GTH5_VERSION_STR \
    GTH5_TO_STR(GTH5_VERSION_MAJOR) "." \
    GTH5_TO_STR(GTH5_VERSION_MINOR) "." \
    GTH5_TO_STR(GTH5_VERSION_PATCH) \
    GTH5_VERSION_ADDITIONAL


#endif // GTH5_VERSION_H
