/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 23.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATA_COMP_H
#define GENH5_DATA_COMP_H

#include "genh5_data/common.h"
#include "genh5_mpl.h"
#include "genh5_exception.h"

namespace GenH5
{

namespace details
{

template <typename Tdata, typename Tbuffer, typename... Args, typename Lambda>
inline void
pushBackHelper(Tdata& data, Tbuffer& buffer, Lambda&& lambda, Args&&... argsIn)
{
    auto args = std::make_tuple(&argsIn...);
    auto size = static_cast<int>(get<0>(args)->size());

    mpl::static_for<sizeof...(Args)>([&](auto const idx){
        if (size != get<idx>(args)->size())
        {
            throw InvalidArgumentError{
                GENH5_MAKE_EXECEPTION_STR_ID("CompData::push_back")
                "Arguments have different number of elements (" +
                std::to_string(size) + " != " +
                std::to_string(get<idx>(args)->size()) + ')'
            };
        }
    });

    data.reserve(size);
    buffer.reserve(size);

    for (int i = 0; i < size; ++i)
    {
        // *(std::cbegin(...)+i) same as [i] but also works for init_lists
        data.append(lambda(std::make_tuple(*(std::cbegin(argsIn)+i)...),
                           buffer.get()));
    }
}

/** DATA VECTOR **/
template<typename...>
class CompDataImpl;

template<typename... Ts>
class CompDataImpl<Comp<Ts...>> : public CommonData<Comp<Ts...>>
{
    using base_class =  CommonData<Comp<Ts...>>;

public:

    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];
    using base_class::push_back;
    using base_class::append;
    using base_class::setValue;
    using base_class::value;
    using base_class::values;
    using base_class::unpack;
    using size_type = typename base_class::size_type;

    static constexpr auto compoundSize() { return sizeof...(Ts); }

    CompDataImpl() {};

    /** constructor **/
    template <typename... Containers,
              traits::if_equal<sizeof...(Ts), sizeof...(Containers)> = true>
    // cppcheck-suppress noExplicitConstructor
    CompDataImpl(Containers&&... containers) noexcept(false)
    {
        push_back(std::forward<Containers>(containers)...);
    }

    /** push_back **/
    // value container types
    template <typename... Containers,
              typename T = Comp<traits::value_t<Containers>...>,
              traits::if_types_equal<T, conversion_t<RComp<Ts...>>> = true>
    void push_back(Containers&&... containers) noexcept(false)
    {
        pushBackHelper(base_class::m_data, base_class::m_buffer,
                          [](auto const& tuple, auto& /*buffer*/){
            return reverseComp(tuple);
        },
        std::forward<Containers>(containers)...);
    }

    // template container types
    template <typename... Containers,
              typename T = Comp<traits::value_t<Containers>...>,
              traits::if_types_differ<T, conversion_t<RComp<Ts...>>> = true,
              traits::if_types_equal<conversion_t<T>,
                                     conversion_t<Comp<Ts...>>> = true>
    void push_back(Containers&&... containers) noexcept(false)
    {
        using GenH5::convert; // ADL

        pushBackHelper(base_class::m_data, base_class::m_buffer,
                          [](auto const& tuple, auto& buffer){
            return convert(tuple, buffer);
        },
        std::forward<Containers>(containers)...);
    }

    // value type arguments
    template <typename... Args,
              traits::if_types_equal<Comp<traits::decay_crv_t<Args>...>,
                                     conversion_t<RComp<Ts...>>> = true>
    void push_back(Args&&... args) noexcept(false)
    {
        base_class::m_data.append(
                    reverseComp(
                        std::make_tuple(std::forward<Args>(args)...)));
    }

    // template type arguments
    template <typename... Args,
              traits::if_types_differ<Comp<traits::decay_crv_t<Args>...>,
                                      conversion_t<RComp<Ts...>>> = true,
              traits::if_types_equal<
                      conversion_t<Comp<traits::decay_crv_t<Args>...>>,
                      conversion_t<Comp<Ts...>>> = true>
    void push_back(Args&&... args) noexcept(false)
    {
        using GenH5::convert; // ADL

        base_class::m_data.append(
                    convert(std::make_tuple(std::forward<Args>(args)...),
                            base_class::m_buffer.get()));
    }

    /** append **/
    // frwd arguemnts to push_back
    template <typename... Args,
              traits::if_equal<sizeof...(Ts), sizeof...(Args)> = true>
    void append(Args&&... args) { push_back(std::forward<Args>(args)...); }

    /** set value **/
    template <size_t tidx,
              typename T,
              traits::if_types_equal<T, traits::comp_element_t<
                                     tidx, conversion_t<RComp<Ts...>>>> = true>
    void setValue(size_type i, T&& value)
    {
        assert(base_class::m_data.size() > i);
        rget<tidx>(base_class::m_data[i]) = std::forward<T>(value);
    }

    template <size_t tidx,
              typename T,
              traits::if_types_differ<T, traits::comp_element_t<
                                     tidx, conversion_t<RComp<Ts...>>>> = true>
    void setValue(size_type i, T&& value)
    {
        using GenH5::convert; // ADL
        this->setValue<tidx>(i,convert(std::forward<T>(value),
                                       rget<tidx>(base_class::m_buffer.get())));
    }

    /** get value **/
    // 1D
    template <size_t tidx,
              typename T = traits::comp_element_t<tidx, Comp<Ts...>>>
    auto getValue(size_type idx) const
    {
        assert(idx < base_class::size());
        using GenH5::convertTo; // ADL

        return convertTo<T>(rget<tidx>(base_class::m_data[idx]));
    }    
    // 2D
    template <size_t tidx,
              typename T = traits::comp_element_t<tidx, Comp<Ts...>>>
    auto getValue(hsize_t idxA, hsize_t idxB) const
    {
        return getValue<tidx, T>(idx(base_class::m_dims, {idxA, idxB}));
    }
    // ND
    template <size_t tidx,
              typename T = traits::comp_element_t<tidx, Comp<Ts...>>>
    auto getValue(Vector<hsize_t> const& idxs) const
    {
        return getValue<tidx, T>(idx(base_class::m_dims, idxs));
    }

    /** unpack **/
    template <typename... Args,
              traits::if_equal<sizeof...(Ts), sizeof...(Args)> = true>
    void unpack(size_type idx, Args&... argsIn) const
    {
        auto& ref = *this; // for lambda capture

        // for iterating more easily over variadic arguments
        auto args = std::make_tuple(&argsIn...);
        mpl::static_for<sizeof...(Ts)>([&](auto const tidx){
            using Tcomp = Comp<traits::decay_crv_t<Args>...>;
            using T = traits::comp_element_t<tidx, Tcomp>;
            *get<tidx>(args) = ref.template getValue<tidx, T>(idx);
        });
    }

    template <typename... Containers,
              traits::if_equal<sizeof...(Ts), sizeof...(Containers)> = true>
    void unpack(Containers&... containersIn) const
    {
        // for iterating more easily over variadic arguments
        auto containers = std::make_tuple(&containersIn...);

        mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            get<idx>(containers)->reserve(base_class::size());
        });

        auto size = base_class::m_data.size();
        auto& ref = *this; // for lambda capture

        // cppcheck-suppress shadowFunction
        for (int idx = 0; idx < size; ++idx)
        {
            mpl::static_for<sizeof...(Ts)>([&](auto const tidx){
                using Tcomp = Comp<traits::decay_crv_t<Containers>...>;
                using Tcontainer = traits::comp_element_t<tidx, Tcomp>;
                using T = traits::value_t<Tcontainer>;
                get<tidx>(containers)->push_back(
                            ref.template getValue<tidx, T>(idx));
            });
        }
    }

    /** get values **/
    template <size_t tidx, typename Container =
                  Vector<traits::convert_to_t<
                         traits::comp_element_t<tidx, Comp<Ts...>>>>>
    auto getValues() const
    {
        using GenH5::convertTo; // ADL

        using T = traits::value_t<Container>;
        Container c;
        c.reserve(base_class::size());
        std::transform(std::cbegin(base_class::m_data),
                       std::cend(base_class::m_data),
                       std::back_inserter(c), [](auto const& value){
            return convertTo<T>(rget<tidx>(value));
        });
        return c;
    }
};

template<typename... Ts>
struct comp_data_helper
{
    using type = details::CompDataImpl<Comp<Ts...>>;
};

template<typename... Ts>
struct comp_data_helper<Comp<Ts...>>
{
    using type = details::CompDataImpl<Comp<Ts...>>;
};

template<typename... Ts>
struct data_helper
{
    using type = details::CompDataImpl<Comp<Ts...>>;
};

template<typename... Ts>
struct data_helper<Comp<Ts...>>
{
    using type = details::CompDataImpl<Comp<Ts...>>;
};

} // namespace details

template<typename... Ts>
using CompData = typename details::comp_data_helper<Ts...>::type;

template <typename... Containers>
inline auto
makeCompData(Containers&&... containers)
{
    return CompData<traits::value_t<Containers>...>{
        std::forward<Containers>(containers)...
    };
}

} // namespace GenH5

#endif // GENH5_DATA_COMP_H
