/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 30.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "genh5_idcomponent.h"

int GenH5::incId(hid_t id)
{
    return H5Iinc_ref(id);
}

int GenH5::decId(hid_t id)
{
    return H5Idec_ref(id);
}

H5I_type_t
GenH5::classType(hid_t id)
{
    return H5Iget_type(id);
}

bool
GenH5::isValidId(hid_t id)
{
    return H5Iis_valid(id) > 0;
}
