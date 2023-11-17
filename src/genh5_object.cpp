/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
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
