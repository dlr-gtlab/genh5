/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 08.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_CONVERSION_GENERIC_H
#define GENH5_CONVERSION_GENERIC_H

#include "genh5_conversion/type.h"
#include "genh5_conversion/buffer.h"
#include "genh5_typetraits.h"
#include "genh5_utils.h"

namespace GenH5
{

template<typename T, size_t N>
auto convert(Array<T, N> const&, buffer_t<Array<T, N>>&);

template<typename T, size_t N>
auto convert(T const (&)[N], buffer_t<Array<T, N>>&);

template<typename T>
hvl_t convert(VarLen<T> const&, buffer_t<VarLen<T>>&);

template<typename... Ts>
auto convert(Comp<Ts...> const&, buffer_t<Comp<Ts...>>&);


template<typename Ttarget,
         typename T = traits::base_t<Ttarget>,
         size_t N = traits::array_size<Ttarget>::value>
auto convertTo(Array<conversion_t<T>, N> const&);

template<typename Ttarget, typename T = traits::base_t<Ttarget>>
auto convertTo(hvl_t);

template <typename Ttarget, typename... Ts>
auto convertTo(Comp<Ts...> const&);


/** ARRAY CONVERSIONS **/
namespace details
{

template<size_t N, typename T, typename A, typename B>
inline auto
convertArrayImpl(A const& values, B& buffer)
{
    using GenH5::convert; // ADL
    GenH5::conversion_t<Array<T, N>> conv{};

    for (size_t i = 0; i < N; ++i)
    {
        conv[i] = convert(values[i], buffer);
    }
    return conv;
}

} // namespace details


template<typename T, size_t N>
inline auto
convert(Array<T, N> const& values, buffer_t<Array<T, N>>& buffer)
{
    return details::convertArrayImpl<N, T>(values, buffer);
}

template<typename T, size_t N>
inline auto
convert(T const (&values)[N], buffer_t<Array<T, N>>& buffer)
{
    return details::convertArrayImpl<N, T>(values, buffer);
}

/** VARLEN CONVERSIONS **/
template<typename T>
inline hvl_t
convert(VarLen<T> const& values, buffer_t<VarLen<T>>& buffer)
{
    using GenH5::convert; // ADL
    buffer.push_back({});
    details::hvl_buffer<T>& hvlB = buffer.back();

    hvlB.data.reserve(values.size());
    for (auto const& val : values)
    {
        hvlB.data.push_back(convert(val, hvlB.buffer));
    }

    hvl_t hvl;
    hvl.len = static_cast<hsize_t>(hvlB.data.size());
    hvl.p   = static_cast<void*>(hvlB.data.data());
    return hvl;
}

/** COMPOUND COMVERSION **/
template<typename... Ts>
inline auto
convert(Comp<Ts...> const& tuple, buffer_t<Comp<Ts...>>& buffer)
{
    using GenH5::convert; // ADL
    conversion_t<Comp<Ts...>> conv;

    mpl::static_for<sizeof...(Ts)>([&](auto const idx){
        rget<idx>(conv) = convert(get<idx>(tuple), rget<idx>(buffer));
    });
    return conv;
}


/** ARRAY REVERSE CONVERSIONS **/
template<typename Ttarget, typename T, size_t N>
inline auto
convertTo(Array<conversion_t<T>, N> const& values)
{
    using GenH5::convertTo; // ADL
    traits::convert_to_t<Array<T, N>> conv;

    for (size_t i = 0; i < N; ++i)
    {
        conv[i] = convertTo<T>(values[i]);
    }
    return conv;
}

/** VARLEN REVERSE CONVERSIONS **/
template<typename Ttarget, typename T> // == VarLen<T>
inline auto
convertTo(hvl_t hvl)
{
    using GenH5::convertTo; // ADL
    traits::convert_to_t<VarLen<T>> conv;

    if (hvl.p)
    {
        conv.reserve(static_cast<int>(hvl.len));
        for (size_t i = 0; i < hvl.len; ++i)
        {
            // interpret buffer as the conversion type (eg. char* for str types)
            auto* t = reinterpret_cast<conversion_t<T>*>(hvl.p) + i;
            // convert to final type
            conv.push_back(convertTo<T>(*t));
        }
    }
    return conv;
}

/** COMPOUND REVERSE COMVERSION **/
template <typename Ttarget, typename... Ts>
inline auto
convertTo(Comp<Ts...> const& tuple)
{
    using GenH5::convertTo; // ADL
    traits::convert_to_t<Ttarget> conv;

    mpl::static_for<sizeof...(Ts)>([&](auto const idx){
        using T = traits::comp_element_t<idx, Ttarget>;
        get<idx>(conv) = convertTo<T>(rget<idx>(tuple));
    });
    return conv;
}

} // namespace GenH5

#endif // GENH5_CONVERSION_GENERIC_H
