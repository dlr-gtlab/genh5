/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_object.h"

using namespace GenH5;

bool
Object::isValid() const noexcept
{
    return H5Iis_valid(id());
}

bool
Object::isValid(hid_t id) noexcept
{
    return H5Iis_valid(id);
}

GenH5::ObjectType
GenH5::Object::type() const noexcept
{
    return H5Iget_type(id());
}
