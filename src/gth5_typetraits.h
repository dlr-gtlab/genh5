/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 15.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_TYPETRAITS_H
#define GTH5_TYPETRAITS_H

#include "gth5_typedefs.h"

namespace GtH5
{

namespace traits
{

// size of an array template type
template <typename T>
struct array_size/* :
        std::integral_constant<size_t, 0> { }*/;

template <typename T, size_t N>
struct array_size<std::array<T, N>> :
        std::integral_constant<size_t, N> { };

// size of a compound template type
template <typename T>
struct comp_size:
        std::integral_constant<size_t, 0> { };

template <typename... Ts>
struct comp_size<Comp<Ts...>>:
        std::integral_constant<size_t, sizeof...(Ts)> { };

template <typename T>
struct conv_base { using type = T; };

template <typename T>
struct conv_base<VarLen<T>> { using type = T; };

template <typename T, size_t N>
struct conv_base<Array<T, N>> { using type = T; };

template <typename T>
using conv_base_t = typename conv_base<T>::type;

template <typename Container>
using value_t = typename std::decay_t<Container>::value_type;

// used to enable template if types differ
template<typename T, typename U>
using if_types_differ = std::enable_if_t<
            !std::is_same<std::decay_t<T>, std::decay_t<U>>::value, bool>;

// used to enable template if types are equal
template<typename T, typename U>
using if_types_equal = std::enable_if_t<
            std::is_same<std::decay_t<T>, std::decay_t<U>>::value, bool>;

// used to enable template if T::value_type exists and is equal to U
template<typename T, typename U>
using if_has_value_type = std::enable_if_t<
            std::is_same<traits::value_t<T>,
                         std::decay_t<U>>::value, bool>;

// used to enable template if A == B
template <size_t A, size_t B>
using if_same_size = std::enable_if_t<A == B, bool>;

/// used to enable template for signed integral types
template <typename Tint>
using if_signed_integral =
            std::enable_if_t<std::is_integral<Tint>::value &&
                             std::is_signed<Tint>::value, bool>;

} // namespace traits

} // namespace GtH5

#endif // GTH5_TYPETRAITS_H
