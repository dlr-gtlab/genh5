/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 07.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_CONVERSION_TYPE_H
#define GTH5_CONVERSION_TYPE_H

#include "gth5_typedefs.h"
#include "gth5_mpl.h"

#define GTH5_DECLARE_CONVERSION_TYPE(NATIVE, CONVERTED) \
    template <> \
    struct GtH5::conversion<NATIVE> : \
            GtH5::details::conversion_impl<NATIVE, CONVERTED> {};

namespace GtH5
{

/* CONVERSION TYPE */
namespace details
{

    // For mapping types to their conversion types
    template<typename Tsrc, typename Tconversion = Tsrc>
    struct conversion_impl
    {
        using type = Tconversion;
    };

} // namespace details

// forward declaration
template<typename ...Ts>
struct conversion;

// using declaration
template<typename ...Ts>
using conversion_t = typename conversion<Ts...>::type;

// single element
template <typename T>
struct conversion<T> :
        details::conversion_impl<T, T> {};

// array element
template <typename T, size_t N>
struct conversion<Array<T, N>> :
        details::conversion_impl<Array<T, N>, Array<conversion_t<T>, N>> {};

// varlen element
template <typename T>
struct conversion<VarLen<T>> :
        details::conversion_impl<VarLen<T>, hvl_t> {};

// compound types
template <typename ...Ts>
struct conversion<Comp<Ts...>>
{
    using type = mpl::reversed_comp_t<Comp<conversion_t<Ts>...>>;
};

} // namespace GtH5

#endif // GTH5_CONVERSION_TYPE_H
