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

    /** constructors **/
    // value type arguments
    template <typename... Args,
              traits::if_types_equal<Comp<traits::decay_crv_t<Args>...>,
                                     conversion_t<RComp<Ts...>>> = true>
    // cppcheck-suppress noExplicitConstructor
    CompData0DImpl(Args&&... args)
    {
        base_class::m_data =
                reverseComp(std::make_tuple(std::forward<Args>(args)...));
    }

    // template type arguments
    template <typename... Args,
              traits::if_types_differ<Comp<traits::decay_crv_t<Args>...>,
                                      conversion_t<RComp<Ts...>>> = true,
              traits::if_types_equal<
                      conversion_t<Comp<traits::decay_crv_t<Args>...>>,
                      conversion_t<Comp<Ts...>>> = true>
    // cppcheck-suppress noExplicitConstructor
    CompData0DImpl(Args&&... args) noexcept(false)
    {
        using GenH5::convert; // ADL

        base_class::m_data =
                convert(std::make_tuple(std::forward<Args>(args)...),
                        base_class::m_buffer.get());
    }

    /** set value **/
    template <size_t tidx,
              typename T,
              traits::if_types_equal<T, traits::comp_element_t<
                                     tidx, conversion_t<RComp<Ts...>>>> = true>
    void setValue(T&& value)
    {
        rget<tidx>(base_class::m_data) = std::forward<T>(value);
    }

    template <size_t tidx,
              typename T,
              traits::if_types_differ<T, traits::comp_element_t<
                                     tidx, conversion_t<RComp<Ts...>>>> = true>
    void setValue(T&& value)
    {
        using GenH5::convert; // ADL
        this->setValue<tidx>(convert(std::forward<T>(value),
                                     rget<tidx>(base_class::m_buffer.get())));
    }

    /** get value **/
    template <size_t tidx,
              typename T = traits::comp_element_t<tidx, Comp<Ts...>>>
    auto getValue() const
    {
        using GenH5::convertTo; // ADL
        return convertTo<T>(rget<tidx>(base_class::m_data));
    }

    /** unpack **/
    template <typename... Args,
              traits::if_same_size<sizeof...(Ts), sizeof...(Args)> = true>
    void unpack(Args&... argsIn) const
    {
        // for iterating more easily over variadic arguments
        auto args = std::make_tuple(&argsIn...);
        auto& ref = *this;

        mpl::static_for<sizeof...(Ts)>([&](auto const tidx){
            using Tcomp = Comp<traits::decay_crv_t<Args>...>;
            using T = traits::comp_element_t<tidx, Tcomp>;
            *get<tidx>(args) = ref.template getValue<tidx, T>();
        });
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
    return CompData0D<traits::decay_crv_t<Args>...>{
        std::forward<Args>(args)...
    };
}

} // namespace GenH5

#endif // GENH5_DATA_COMP0D_H
