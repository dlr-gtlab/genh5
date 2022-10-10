/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 17.08.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATA_COMP0D_H
#define GENH5_DATA_COMP0D_H

#include "genh5_data/common0d.h"
#include "genh5_exception.h"

namespace GenH5
{

namespace details
{

/** DATA 0D **/
template<typename...>
class CompData0DImpl;

template<typename... Ts>
class CompData0DImpl<Comp<Ts...>> : public CommonData0D<Comp<Ts...>>
{
    using base_class =  CommonData0D<Comp<Ts...>>;

public:

    using base_class::base_class;
    using base_class::operator=;
    using base_class::value;

    static constexpr auto compoundSize() { return sizeof...(Ts); }

    CompData0DImpl() = default;

    /** conversion constructors **/
    template <typename... Us,
              traits::if_same_size<sizeof...(Ts), sizeof...(Us)> = true>
    explicit CompData0DImpl(Us&&... argsIn) noexcept(false)
    {
        using GenH5::convert; // ADL

        // for iterating more easily over variadic arguments
        auto args = std::make_tuple(&argsIn...);

        mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            auto* arg = get<idx>(args);

            auto& data = rget<idx>(base_class::m_data);
            data = convert(*arg, rget<idx>(base_class::m_buffer()));
        });
    }

    /** unpack **/
    void unpack(Ts&... argsIn) const
    {
        using GenH5::convertTo; // ADL

        // for iterating more easily over variadic arguments
        auto args = std::make_tuple(&argsIn...);

        mpl::static_for<sizeof...(Ts)>([&](auto const tidx){
            using T = std::tuple_element_t<tidx, Comp<Ts...>>;
            auto* arg = get<tidx>(args);
            *arg = convertTo<T>(rget<tidx>(base_class::m_data));
        });
    }

    /** get value **/
    template <size_t tidx>
    auto getValue() const
    {
        using GenH5::convertTo; // ADL
        using T = std::tuple_element_t<tidx, Comp<Ts...>>;
        return convertTo<T>(rget<tidx>(base_class::m_data));
    }
};

template<typename... Ts>
struct comp_data_0d_helper
{
    using type = details::CompData0DImpl<Comp<Ts...>>;
};

template<typename... Ts>
struct comp_data_0d_helper<Comp<Ts...>>
{
    using type = details::CompData0DImpl<Comp<Ts...>>;
};

template<typename... Ts>
struct data_0d_helper
{
    using type = details::CompData0DImpl<Comp<Ts...>>;
};

template<typename... Ts>
struct data_0d_helper<Comp<Ts...>>
{
    using type = details::CompData0DImpl<Comp<Ts...>>;
};

} // namespace details

template<typename... Ts>
using CompData0D = typename details::comp_data_0d_helper<Ts...>::type;

template <typename... Args>
inline auto
makeCompData0D(Args&&... args)
{
    return CompData0D<traits::value_t<Args>...>{
        std::forward<Args>(args)...
    };
}

} // namespace GenH5

#endif // GENH5_DATA_COMP0D_H
