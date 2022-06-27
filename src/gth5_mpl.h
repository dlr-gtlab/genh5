/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 15.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_MPL_H
#define GTH5_MPL_H

#include "gth5_typedefs.h"

namespace GtH5
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

template<size_t N, typename Lambda, std::size_t ... Is>
constexpr void
static_rfor_impl(Lambda&& f, std::index_sequence<Is...>)
{
    struct Idx {};
    // unpack into init list for "looping" in correct order wo recursion
    (void)std::initializer_list<Idx> {
        ((void)f(std::integral_constant<unsigned, Is>(),
                 std::integral_constant<unsigned, N-Is-1>()), Idx{})...
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

/// reverses a compound template type
template<typename T>
using reversed_comp_t = typename reversed_comp<T>::type;

// static for loop for compound types
/**
 * @brief Calls a method for each n in N
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

/**
 * @brief Calls a method for each n in N. Can be used to iterate backwards.
 * @param f Function to call for each iteration.
 * @tparam N Number of iterations
 * @tparam Lambda Function to call. Must have two parameters. First
 * represents * the current forward index (i.e. n), last the current
 * reversed index (i.e. N - n - 1)
 */
template<unsigned N, typename Lambda>
constexpr void
static_rfor(Lambda&& f)
{
    details::static_rfor_impl<N>(std::forward<Lambda>(f),
                                 std::make_index_sequence<N>());
}

} // namespace mpl

} // namespace GtH5

#endif // GTH5_MPL_H
