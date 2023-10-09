/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 15.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_MPL_H
#define GENH5_MPL_H

#include <utility>
#include <stddef.h>

namespace GenH5
{

namespace details
{

template<typename>
struct reversed_comp_empty;

template <template<typename...> class Tcomp, typename... Ts>
struct reversed_comp_empty<Tcomp<Ts...>>
{
    using type = Tcomp<>;
};

template<typename T>
using reversed_comp_empty_t = typename reversed_comp_empty<T>::type;

template<typename Lambda, std::size_t ... Is>
constexpr void
static_for_impl(Lambda&& f, std::index_sequence<Is...>)
{
    struct Idx {};
    // unpack into init list for "looping" in correct order wo recursion
    (void)std::initializer_list<Idx > {
        ((void)f(std::integral_constant<unsigned, Is>()), Idx{})...
    };
}

} // namespace details

namespace mpl
{

// reversing a compound type
template<typename Tinput,
         typename Toutput = details::reversed_comp_empty_t<Tinput>>
struct reversed_comp;

template<template <typename...> class Tcomp, typename... Ts>
struct reversed_comp<details::reversed_comp_empty_t<Tcomp<Ts...>>,
                     Tcomp<Ts...>>
{
    using type = Tcomp<Ts...>;
};

template<template<typename...> class Tcomp, typename T1, typename... Tother,
         typename... done>
struct reversed_comp<Tcomp<T1, Tother...>, Tcomp<done...>>
{
    using type = typename reversed_comp<Tcomp<Tother...>,
                                        Tcomp<T1, done...>>::type;
};

// reverses a compound template type
template<typename T>
using reversed_comp_t = typename reversed_comp<T>::type;

/**
 * @brief Calls a method for each n in N. Static for loop for compound types
 * @param f Function to call for each iteration.
 * @tparam N Number of iterations
 * @tparam Lambda Function to call. Must have a single parameter
 * representing the current index
 */
template<unsigned N, typename Lambda>
constexpr void
static_for(Lambda&& f)
{
    details::static_for_impl(std::forward<Lambda>(f),
                             std::make_index_sequence<N>());
}

} // namespace mpl

} // namespace GenH5

#endif // GENH5_MPL_H
