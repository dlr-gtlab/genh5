/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 27.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_version.h"
#include "H5public.h"

GenH5::Version
GenH5::Version::current()
{
    return {GENH5_VERSION_MAJOR, GENH5_VERSION_MINOR, GENH5_VERSION_PATCH};
}

const char*
GenH5::Version::subrelease()
{
    return GENH5_VERSION_ADDITIONAL;
}

GenH5::Version
GenH5::Version::hdf5()
{
    return {H5_VERS_MAJOR, H5_VERS_MINOR, H5_VERS_RELEASE};
}

const char*
GenH5::Version::subreleaseHdf5()
{
    return H5_VERS_SUBRELEASE;
}
