/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_object.h"
#include "genh5_idcomponent.h"

using namespace GenH5;

bool
Object::isValid() const noexcept
{
    return isValidId(id());
}

bool
Object::isValid(hid_t id) noexcept
{
    return isValidId(id);
}
