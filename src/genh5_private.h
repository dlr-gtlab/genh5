/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 29.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_PRIVATE_H
#define GENH5_PRIVATE_H

#include "genh5_typetraits.h"

#include "H5Ipublic.h"

#include <utility>

namespace GenH5
{
namespace details
{

/// Helper function for creating T
template <typename T,
          typename Ex,
          typename Functor,
          typename CharType>
inline T make(Functor const& functor, CharType&& msg)
{
    hid_t id = functor();
    if (id < 0)
    {
        throw Ex{std::forward<CharType>(msg)};
    }
    return T::fromId(id);
}

} // namespace details

} // namespace GenH5

#endif // GENH5_PRIVATE_H
