/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 07.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_CONVERSION_TYPE_H
#define GENH5_CONVERSION_TYPE_H

#include "genh5_typedefs.h"

#include "H5Tpublic.h"

#define GENH5_DECLARE_CONVERSION_TYPE(NATIVE, CONVERTED) \
    template <> \
    struct GenH5::conversion<NATIVE> : \
            GenH5::details::conversion_impl<NATIVE, CONVERTED> {};

namespace GenH5
{

/** CONVERSION TYPE **/
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
template<typename T>
using conversion_t = typename conversion<T>::type;

// single element
template <typename T>
struct conversion<T> :
        details::conversion_impl<T, T> {};

// array element
template <typename T, size_t N>
struct conversion<Array<T, N>> :
        details::conversion_impl<Array<T, N>, Array<conversion_t<T>, N>> {};

template <typename T, size_t N>
struct conversion<T[N]> :
        details::conversion_impl<T[N], Array<conversion_t<T>, N>> {};

// varlen element
template <typename T>
struct conversion<VarLen<T>> :
        details::conversion_impl<VarLen<T>, hvl_t> {};

// compound types
template <typename ...Ts>
struct conversion<Comp<Ts...>>
{
    using type = RComp<conversion_t<Ts>...>;
};

/** CONVERSION CONTAINER TYPE **/
// by default vector of T
template<typename T>
struct conversion_container
{
    using type = Vector<conversion_t<T>>;
};

template<typename T>
using conversion_container_t = typename conversion_container<T>::type;

} // namespace GenH5

#endif // GENH5_CONVERSION_TYPE_H
