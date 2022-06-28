/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 27.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_version.h"

GenH5::Version
GenH5::Version::current()
{
    return {GENH5_VERSION_MAJOR, GENH5_VERSION_MINOR, GENH5_VERSION_PATCH};
}
