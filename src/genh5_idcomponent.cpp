/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 *  Created on: 30.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "genh5_idcomponent.h"
#include "H5Ipublic.h"

int
GenH5::incId(hid_t id)
{
    return H5Iinc_ref(id);
}

int
GenH5::decId(hid_t id)
{
    return H5Idec_ref(id);
}

GenH5::IdType
GenH5::classType(hid_t id)
{
    return static_cast<IdType>(H5Iget_type(id));
}

bool
GenH5::isValidId(hid_t id)
{
    return H5Iis_valid(id) > 0;
}

int
GenH5::refCount(hid_t id)
{
    return H5Iget_ref(id);
}
