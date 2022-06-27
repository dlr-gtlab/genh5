/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 08.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_CONVERSION_GENERIC_H
#define GENH5_CONVERSION_GENERIC_H

#include "type.h"
#include "buffer.h"
#include "genh5_typetraits.h"

namespace GenH5
{

/*
 * FORWARD DECLARATIONS CONVERSIONS
 */

template<typename T, size_t N>
auto convert(Array<T, N> const&, buffer_t<Array<T, N>>&);

template<typename T>
hvl_t convert(VarLen<T> const&, buffer_t<VarLen<T>>&);

template<typename... Ts>
auto convert(Comp<Ts...> const&, buffer_t<Comp<Ts...>>&);

template<typename Ttarget, typename T = traits::value_t<Ttarget>>
auto convertTo(Array<conversion_t<T>,
               traits::array_size<Ttarget>::value> const&);

template<typename VarLenT>
VarLenT convertTo(hvl_t);

template <typename Ttarget, typename... Ts>
Ttarget convertTo(Comp<Ts...> const&);

/*
 * IMPLEMENTATIONS
 */

/* ARRAY CONVERSIONS */
template<typename T, size_t N>
inline auto
convert(Array<T, N> const& values, buffer_t<Array<T, N>>& buffer)
{
    using GenH5::convert;

    GenH5::conversion_t<Array<T, N>> conv{};

    for (auto i = 0; i < values.size(); ++i)
    {
        conv[i] = convert(values[i], buffer);
    }

    return conv;
}

/* VARLEN CONVERSIONS */
template<typename T>
inline hvl_t
convert(VarLen<T> const& values, buffer_t<VarLen<T>>& buffer)
{
    using GenH5::convert;

    buffer.push_back({});
    auto& hvlB = buffer.back();

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

/* COMPOUND COMVERSION */
template<typename... Ts>
inline auto
convert(Comp<Ts...> const& tuple, buffer_t<Comp<Ts...>>& buffer)
{
    using GenH5::convert; // ADL

    conversion_t<Comp<Ts...>> conv;

    mpl::static_rfor<sizeof...(Ts)>([&](auto const idx, auto const ridx){
        std::get<ridx>(conv) = convert(std::get<idx>(tuple),
                                       std::get<ridx>(buffer));
    });

    return conv;
}

/* ARRAY REVERSE CONVERSIONS */
template<typename Ttarget, typename T>
inline auto
convertTo(Array<conversion_t<T>,
          traits::array_size<Ttarget>::value> const& values)
{
    using GenH5::convertTo;

    Ttarget conv;

    for (auto i = 0; i < values.size(); ++i)
    {
        conv[i] = convertTo<T>(values[i]);
    }

    return conv;
}

/* VARLEN REVERSE CONVERSIONS */
template<typename VarLenT> // == VarLen<T>
inline VarLenT
convertTo(hvl_t hvl)
{
    using GenH5::convertTo;

    VarLenT conv;

    if (hvl.p)
    {
        conv.reserve(hvl.len);
        for (size_t i = 0; i < hvl.len; ++i)
        {
            // varlen type must be STL like
            using ValueType = traits::value_t<VarLenT>;
            // interpret buffer as the conversion type (eg. char* for str types)
            auto* t = reinterpret_cast<conversion_t<ValueType>*>(hvl.p) + i;
            // convert to final type
            conv.push_back(convertTo<ValueType>(*t));
        }
    }

    return conv;
}

/* COMPOUND REVERSE COMVERSION */
template <typename Ttarget, typename... Ts>
inline Ttarget
convertTo(Comp<Ts...> const& tuple)
{
    using GenH5::convertTo; // ADL

    Ttarget conv;

    mpl::static_rfor<sizeof...(Ts)>([&](auto const idx, auto const ridx){
        using T = std::tuple_element_t<idx, Ttarget>;
        std::get<idx>(conv) = convertTo<T>(std::get<ridx>(tuple));
    });

    return conv;
}

} // namespace GenH5

#endif // GENH5_CONVERSION_GENERIC_H
