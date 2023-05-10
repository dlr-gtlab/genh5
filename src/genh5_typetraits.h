/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 15.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_TYPETRAITS_H
#define GENH5_TYPETRAITS_H

#include "genh5_typedefs.h"

namespace GenH5
{

namespace traits
{

// size of an array template type
template <typename T>
struct array_size :
        std::integral_constant<size_t, 0> { };

template <typename T, size_t N>
struct array_size<std::array<T, N>> :
        std::integral_constant<size_t, N> { };

template <typename T, size_t N>
struct array_size<T[N]> :
        std::integral_constant<size_t, N> { };

// size of a compound template type
template <typename T>
struct comp_size:
        std::integral_constant<size_t, 0> { };

template <typename... Ts>
struct comp_size<Comp<Ts...>> :
        std::integral_constant<size_t, sizeof...(Ts)> { };

// element of a compound template type
template <size_t idx, typename T>
using comp_element_t = std::tuple_element_t<idx, T>;

template <size_t idx, typename T, size_t size = std::tuple_size<T>::value>
using rcomp_element_t = std::tuple_element_t<(size - 1 - idx), T>;

// for deducing return type of convert to methods
template <typename... Ts>
struct convert_to;

template <typename T>
using convert_to_t = typename convert_to<T>::type;

template <typename T>
struct convert_to<T> { using type = T; };

template <typename T, size_t N>
struct convert_to<Array<T, N>> { using type = Array<convert_to_t<T>, N>; };

template <typename T, size_t N>
struct convert_to<T[N]> { using type = Array<convert_to_t<T>, N>; };

template <typename T>
struct convert_to<VarLen<T>> { using type = VarLen<convert_to_t<T>>; };

template <typename... Ts>
struct convert_to<Comp<Ts...>> { using type = Comp<convert_to_t<Ts>...>; };

// access underlying type (for array, varlens and comps)
template <typename T>
struct base { using type = T; };

template <typename T, size_t N>
struct base<Array<T, N>> { using type = T; };

template <typename T, size_t N>
struct base<T[N]> { using type = T; };

template <typename T>
struct base<VarLen<T>> { using type = T; };

template <typename T>
using base_t = typename base<T>::type;

// decay that keep pointers
template<typename T>
struct decay_crv { using type = std::remove_cv_t<std::remove_reference_t<T>>; };

template<typename T>
using decay_crv_t = typename decay_crv<T>::type;

// access value_type of Container
template <typename Container>
using value_t = typename decay_crv_t<Container>::value_type;

// helper struct to check if a type exists
template<typename T, class R = void>
struct enable_if_type { using type = R; };

template<typename T, class R = void>
using enable_if_type_t = typename enable_if_type<T, R>::type;

// to check if T::template_type exists
template<typename T, class Enable = void>
struct has_template_type :
        std::false_type {};

template<typename T>
struct has_template_type<T, enable_if_type_t<typename T::template_type>>:
        std::true_type {};

// used to enable template if types differ
template<typename T, typename U>
using if_types_differ = std::enable_if_t<
            !std::is_same<convert_to_t<decay_crv_t<T>>,
                          convert_to_t<decay_crv_t<U>>>::value, bool>;

// used to enable template if types are equal
template<typename T, typename U>
using if_types_equal = std::enable_if_t<
            std::is_same<convert_to_t<decay_crv_t<T>>,
                         convert_to_t<decay_crv_t<U>>>::value, bool>;

// used to enable template if T::value_type exists and is equal to U
template<typename T, typename U>
using if_value_types_equal = std::enable_if_t<
            std::is_same<value_t<T>, decay_crv_t<U>>::value ||
            std::is_same<value_t<T>, convert_to_t<decay_crv_t<U>>>::value,
            bool>;

// used to enable template if A == B
template <size_t A, size_t B>
using if_equal = std::enable_if_t<(A == B), bool>;

// used to enable template if A > B
template <size_t A, size_t B>
using if_greater_than = std::enable_if_t<(A > B), bool>;

// used to enable template for signed integral types
template <typename Tint>
using if_signed_integral =
            std::enable_if_t<std::is_integral<Tint>::value &&
                             std::is_signed<Tint>::value, bool>;

template <typename T>
using if_not_pointer =
            std::enable_if_t<!std::is_pointer<T>::value, bool>;

// used to check if T::template_type does not exist
template <typename T>
using if_has_not_template_type =
            std::enable_if_t<!has_template_type<decay_crv_t<T>>::value, bool>;

// used to check if T is convertible to U
template <typename T, typename U>
using if_convertible =
    std::enable_if_t<std::is_convertible<T, U>::value, bool>;

// used to check if T is not convertible to U
template <typename T, typename U>
using if_not_convertible =
    std::enable_if_t<!std::is_convertible<T, U>::value, bool>;


} // namespace traits

} // namespace GenH5

#endif // GENH5_TYPETRAITS_H
