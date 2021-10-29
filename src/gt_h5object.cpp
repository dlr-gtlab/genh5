/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5object.h"

bool
GtH5Object::isValid() const
{
    return H5Iis_valid(this->id());
}

bool
GtH5Object::isValid(int64_t id)
{
    return H5Iis_valid(id);
}

