/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 17.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_UTILS_H
#define GTH5_UTILS_H

#include "gth5_mpl.h"
#include "gth5_typetraits.h"
#include "gth5_exports.h"
#include "gth5_exception.h"

namespace GtH5
{

/**
 * @brief Product of all elements in a container (e.g. dataspace)
 * @param c Container
 * @return number of elements
 */
template<typename Container, typename Tout = traits::value_t<Container>>
inline Tout
prod(Container const& c)
{
    using Tin = traits::value_t<Container>;
    return std::accumulate(std::cbegin(c), std::cend(c),
                           Tout(!(std::cbegin(c) == std::cend(c))),
                           [](Tout prod, Tin dim) {
        return prod * (dim);
    });
}

template<typename Tout, typename Container>
inline auto
prod(Container const& container)
{
    return prod<Container, Tout>(container);
}

namespace details
{

template<typename T, size_t... Is>
inline auto
reverseCompImpl(T&& t, std::index_sequence<Is...>)
{
    return std::make_tuple(std::get<sizeof...(Is) - Is - 1>(
                               std::forward<T>(t))...);
}

} // namespace details

template<typename T, typename TT = std::decay_t<T>,
         size_t N = traits::comp_size<TT>::value>
inline auto
reverseComp(T&& t)
{
    return details::reverseCompImpl(std::forward<TT>(t),
                                    std::make_index_sequence<N>());
}

/* MAKE HELPERS */
template <size_t N, typename Container>
inline auto
makeArray(Container&& container)
{
    Array<traits::value_t<Container>, N> array;
    std::copy_n(std::cbegin(container),
                std::min(N, static_cast<size_t>(container.size())),
                std::begin(array));
    return array;
}

template <typename Container>
inline auto
makeVarLen(Container&& container)
{
    VarLen<traits::value_t<Container>> varlen;
    varlen.reserve(container.size());
    std::copy(std::cbegin(container), std::cend(container),
              std::back_inserter(varlen));
    return varlen;
}

template <typename... Containers>
inline auto
makeComp(Containers&&... containersIn) noexcept(false)
{
    using Ttarget = Comp<traits::value_t<Containers>...>;

    Vector<Ttarget> tuples;

    // for iterating more easily over variadic arguments
    auto containers = std::make_tuple(&containersIn...);
    auto size = std::get<0>(containers)->size();
    tuples.resize(size);

    mpl::static_for<sizeof... (Containers)>([&](auto const idx){
        auto* container = std::get<idx>(containers);
        if (size != container->size())
        {
            throw InvalidArgumentError{"Containers have different number "
                                       "of elements!"};
        }

        size_t i = 0;
        for (auto const& value : *container)
        {
            std::get<idx>(tuples[i++]) = value;
        }
    });

    return tuples;
}

/* UNPACK */
template <typename T, size_t N, typename Container>
inline void
unpack(Array<T, N> const& array, Container& container)
{
    container.reserve(N);
    std::copy(std::cbegin(array), std::cend(array),
              std::back_inserter(container));
}

template <typename T, typename Container>
inline void
unpack(VarLen<T> const& varlen, Container& container)
{
    container.reserve(varlen.size());
    std::copy(std::cbegin(varlen), std::cend(varlen),
              std::back_inserter(container));
}

template <typename... Ts>
inline void
unpack(Comp<Ts...> const& tuple, Ts&... valuesIn)
{
    // for iterating more easily over variadic arguments
    auto values = std::make_tuple(&valuesIn...);

    mpl::static_for<sizeof...(Ts)>([&](auto const idx){
        *std::get<idx>(values) = std::get<idx>(tuple);
    });
}

template <typename Tuple, typename... Containers>
inline void
unpack(Vector<Tuple> const& tuples, Containers&... containersIn)
{
    // for iterating more easily over variadic arguments
    auto containers = std::make_tuple(&containersIn...);

    mpl::static_for<sizeof... (Containers)>([&](auto const idx){
        std::get<idx>(containers)->reserve(tuples.size());
    });

    for (auto const& tuple : qAsConst(tuples))
    {
        mpl::static_for<traits::comp_size<Tuple>::value>([&](auto const idx){
            std::get<idx>(containers)->push_back(std::get<idx>(tuple));
        });
    }
}

template <typename Nested, typename... Containers>
inline void
unpackNested(Nested&& nested, Containers&... containersIn)
{
    // for iterating more easily over variadic arguments
    auto containers = std::make_tuple(&containersIn...);

    mpl::static_for<sizeof... (Containers)>([&](auto const idx){
        std::get<idx>(containers)->resize(nested.size());
    });

    for (size_t i = 0; i < nested.size(); ++i)
    {
        unpack(nested[i], containersIn[i]...);
    }
}

} // namespace GtH5

#endif // GTH5_UTILS_H
