/* GenH5
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Author: Martin Siggel
 */

#ifndef GENH5_HOOKS_H
#define GENH5_HOOKS_H

#include <functional>

namespace GenH5
{

enum HookType
{
    PreRead,
    PostRead,
    PreWrite,
    PostWrite
};

using Hook = std::function<void()>;

} // namespace GenH%

#endif // GENH5_HOOKS_H
