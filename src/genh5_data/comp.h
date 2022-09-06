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
    using base_class::value;
    using base_class::values;
    using base_class::unpack;
    using size_type = typename base_class::size_type;

    static constexpr auto compoundSize() { return sizeof...(Ts); }

    CompDataImpl() = default;

    /** conversion constructors **/
    template <typename... Containers,
              traits::if_same_size<sizeof...(Ts), sizeof...(Containers)> = true>
    explicit CompDataImpl(Containers&&... containersIn) noexcept(false)
    {
        using GenH5::convert; // ADL

        // for iterating more easily over variadic arguments
        auto containers = std::make_tuple(&containersIn...);
        auto size = get<0>(containers)->size();
        base_class::m_data.resize(size);

        mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            auto* container = get<idx>(containers);
            if (size != container->size())
            {
                throw InvalidArgumentError{"Containers have different number "
                                           "of elements!"};
            }

            size_type i = 0;
            for (auto const& value : *container)
            {
                auto& data = rget<idx>(base_class::m_data[i++]);
                data = convert(value, rget<idx>(base_class::m_buffer));
            }
        });
    }

    /** unpack single value **/
    auto unpack(size_type idx, Ts&... argsIn) const
    {
        assert(idx < base_class::size());
        using GenH5::convertTo; // ADL

        // for iterating more easily over variadic arguments
        auto args = std::make_tuple(&argsIn...);
        auto const& value = base_class::m_data[idx];

        mpl::static_for<sizeof...(Ts)>([&](auto const tidx){
            using T = std::tuple_element_t<tidx, Comp<Ts...>>;
            auto* arg = get<tidx>(args);
            *arg = convertTo<T>(rget<tidx>(value));
        });
    }

    /** get value **/
    // 1D
    template <size_t tidx>
    auto getValue(size_type idx) const
    {
        assert(idx < base_class::size());
        using GenH5::convertTo; // ADL

        using T = std::tuple_element_t<tidx, Comp<Ts...>>;
        return convertTo<T>(rget<tidx>(base_class::m_data[idx]));
    }    
    // 2D
    template <size_t tidx>
    auto getValue(hsize_t idxA, hsize_t idxB) const
    {
        return getValue<tidx>(idx(base_class::m_dims, {idxA, idxB}));
    }
    // ND
    template <size_t tidx>
    auto getValue(Vector<hsize_t> const& idxs) const
    {
        return getValue<tidx>(idx(base_class::m_dims, idxs));
    }

    /** unpack **/
    template <typename... Containers,
              traits::if_same_size<sizeof...(Ts), sizeof...(Containers)> = true>
    void unpack(Containers&... containersIn) const
    {
        using GenH5::convertTo; // ADL

        // for iterating more easily over variadic arguments
        auto containers = std::make_tuple(&containersIn...);

        mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            get<idx>(containers)->reserve(base_class::size());
        });

        for (auto const& value : qAsConst(base_class::m_data))
        {
            mpl::static_for<sizeof...(Ts)>([&](auto const idx){
                using T = std::tuple_element_t<idx, Comp<Ts...>>;
                auto* container = get<idx>(containers);
                container->push_back(convertTo<T>(rget<idx>(value)));
            });
        }
    }

    /** get values **/
    template <size_t tidx, typename Container =
                  Vector<traits::convert_to_t<
                         std::tuple_element_t<tidx, Comp<Ts...>>>>>
    auto getValues() const
    {
        using GenH5::convertTo; // ADL

        using T = std::tuple_element_t<tidx, Comp<Ts...>>;
        Container c;
        c.reserve(base_class::size());
        std::transform(std::cbegin(base_class::m_data),
                       std::cend(base_class::m_data),
                       std::back_inserter(c), [](auto const& value){
            return convertTo<T>(rget<tidx>(value));
        });
        return c;
    }

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    /** deserialize **/
    using base_class::deserialize;
    using base_class::deserializeIdx;

    [[deprecated("Use CompData<T>::unpack instead")]]
    void deserializeIdx(size_type idx, Ts&... argsIn) const
    {
        return unpack(idx, argsIn...);
    }

    template <typename... Containers,
              traits::if_same_size<sizeof...(Ts), sizeof...(Containers)> = true>
    [[deprecated("Use CompData<T>::unpack instead")]]
    void deserialize(Containers&... containersIn) const
    {
        return unpack(containersIn...);
    }
#endif
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
