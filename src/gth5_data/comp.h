/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 23.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATA_COMP_H
#define GTH5_DATA_COMP_H

#include "gth5_data/base.h"
#include "gth5_mpl.h"

namespace GtH5
{

namespace details
{

template<typename...>
class CompDataImpl;

template<typename... Ts>
class CompDataImpl<Comp<Ts...>> : public AbstractData<Comp<Ts...>>
{
    using base_class =  AbstractData<Comp<Ts...>>;

public:

    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];

    using base_class::deserialize;
    using base_class::deserializeIdx;
    using base_class::size;

    static constexpr auto compoundSize() { return sizeof...(Ts); }

    CompDataImpl() = default;

    template <typename... Containers,
              traits::if_same_size<sizeof...(Ts), sizeof...(Containers)> = true>
    explicit CompDataImpl(Containers&&... containersIn) noexcept(false)
    {
        using GtH5::convert; // ADL

        // for iterating more easily over variadic arguments
        auto containers = std::make_tuple(&containersIn...);
        auto size = std::get<0>(containers)->size();
        base_class::m_data.resize(size);

        mpl::static_rfor<sizeof...(Containers)>([&](auto const idx,
                                                    auto const ridx){
            auto* container = std::get<idx>(containers);
            if (size != container->size())
            {
                throw std::range_error("HDF5: Creating compund data failed! "
                                       "Arguments must have the same number of "
                                       "elements!");
            }

            typename base_class::size_type i = 0;
            for (auto const& value : *container)
            {
                auto& data = std::get<ridx>(base_class::m_data[i++]);
                data = convert(value, std::get<ridx>(base_class::m_buffer));
            }
        });
    }

    void deserializeIdx(int idx, Ts&... argsIn)
    {
        using GtH5::convertTo; // ADL

        // for iterating more easily over variadic arguments
        auto args = std::make_tuple(&argsIn...);

        mpl::static_rfor<sizeof...(Ts)>([&](auto const argidx,
                                            auto const ridx){
            using T = std::tuple_element_t<argidx, Comp<Ts...>>;
            auto* arg = std::get<argidx>(args);
            *arg = convertTo<T>(std::get<ridx>(base_class::m_data[idx]));
        });
    }

    template <typename... Containers,
              traits::if_same_size<sizeof...(Ts), sizeof...(Containers)> = true>
    void deserialize(Containers&... containersIn)
    {
        using GtH5::convertTo; // ADL

        // for iterating more easily over variadic arguments
        auto containers = std::make_tuple(&containersIn...);

        mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            std::get<idx>(containers)->reserve(size());
        });

        for (auto const& value : qAsConst(base_class::m_data))
        {
            mpl::static_rfor<sizeof...(Ts)>([&](auto const idx,
                                                auto const ridx){
                using T = std::tuple_element_t<idx, Comp<Ts...>>;
                auto* container = std::get<idx>(containers);
                container->push_back(convertTo<T>(std::get<ridx>(value)));
            });
        }
    }
};

} // namespace details

template<typename... Ts>
class CompData : public details::CompDataImpl<Comp<Ts...>>
{
    using base_class =  details::CompDataImpl<Comp<Ts...>>;
public:
    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];
};

template<typename... Ts>
class CompData<Comp<Ts...>> : public details::CompDataImpl<Comp<Ts...>>
{
    using base_class =  details::CompDataImpl<Comp<Ts...>>;
public:
    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];
};

template <typename... Containers>
inline auto
makeCompData(Containers&&... containers)
{
    return CompData<traits::value_t<Containers>...>{
        std::forward<std::decay_t<Containers>>(containers)...
    };
}

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
template<typename... Ts>
class GtH5Data : public GtH5::details::CompDataImpl<GtH5::Comp<Ts...>>
{
    using base_class = GtH5::details::CompDataImpl<GtH5::Comp<Ts...>>;
public:
    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];

    auto data() const { return base_class::c(); }
    auto data() { return base_class::c(); }
};
#endif

#endif // GTH5_DATA_COMP_H
