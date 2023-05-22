/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 17.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_UTILS_H
#define GENH5_UTILS_H

#include "genh5_mpl.h"
#include "genh5_typetraits.h"
#include "genh5_exception.h"

namespace GenH5
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
        return prod * dim;
    });
}

template<typename Tout, typename Container>
inline auto
prod(Container const& container)
{
    return prod<Container, Tout>(container);
}

/** IDX **/
inline hsize_t
idx(Dimensions const& dims, Vector<hsize_t> const& idxs)
{
    auto size = dims.size();
    if (size != idxs.size())
    {
        throw InvalidArgumentError{
            GENH5_MAKE_EXECEPTION_STR() "nDims does not equal idx list size (" +
            std::to_string(size) + " != " + std::to_string(idxs.size()) + ')'
        };
    }

    hsize_t idx = 0;
    for (auto i = 0; i < size; ++i)
    {
        idx += idxs[i] * std::accumulate(std::cbegin(dims) + i + 1,
                                         std::cend(dims), 1ull,
                                         [](auto prod, auto dim){
            return prod * dim;
        });
    }
    return idx;
}

/** GET  **/
template <size_t idx,
          typename Ttuple,
          size_t size = std::tuple_size<traits::decay_crv_t<Ttuple>>::value>
inline decltype(auto)
rget(Ttuple&& tuple)
{
    return std::get<size-1-idx>(std::forward<Ttuple>(tuple));
}

template <size_t idx, typename Ttuple>
inline decltype(auto)
get(Ttuple&& tuple)
{
    return std::get<idx>(std::forward<Ttuple>(tuple));
}

namespace details
{

template<typename T, size_t... Is>
inline auto
reverseCompImpl(T&& t, std::index_sequence<Is...>)
{
    return std::make_tuple(get<sizeof...(Is) - Is - 1>(std::forward<T>(t))...);
}

} // namespace details

template<typename T,
         size_t N = traits::comp_size<traits::decay_crv_t<T>>::value>
inline auto
reverseComp(T&& t)
{
    return details::reverseCompImpl(std::forward<T>(t),
                                    std::make_index_sequence<N>());
}

/** MAKE **/
template <size_t N,
          typename Container,
          typename R = Array<traits::value_t<Container>, N>>
inline auto
makeArray(Container&& container)
{
    R array{};
    std::copy_n(std::cbegin(container),
                std::min(N, static_cast<size_t>(container.size())),
                std::begin(array));
    return array;
}

template <size_t N, typename Container,
          typename C = traits::value_t<Container>,
          typename R = Vector<Array<traits::value_t<C>, N>>>
inline auto
makeArrays(Container&& container)
{
    R arrays;
    arrays.reserve(container.size());
    std::transform(std::cbegin(container), std::cend(container),
                   std::back_inserter(arrays), [](C const& c){
        return makeArray<N>(c);
    });
    return arrays;
}

template <size_t N, typename Container,
          typename C = traits::value_t<Container>,
          typename R = Vector<Array<C, N>>>
inline auto
makeArraysFromList(Container&& container) noexcept(false)
{
    if (container.size() % N != 0)
    {
        throw InvalidArgumentError{
            GENH5_MAKE_EXECEPTION_STR()
            "Failed to split container into equally sized arrays (" +
            std::to_string(container.size()) + " % " +
            std::to_string(N) + " != 0)"
        };
    }
    int length = container.size() / N;
    R arrays;
    arrays.reserve(length);
    for (int i = 0; i < length; ++i)
    {
        Array<C, N> array{};
        std::copy(std::cbegin(container) + i * N,
                  std::cbegin(container) + (i + 1) * N,
                  std::begin(array));
        arrays.push_back(std::move(array));
    }
    return arrays;
}

template <typename Container,
          typename R = VarLen<traits::value_t<Container>>>
inline auto
makeVarLen(Container&& container)
{
    R varlen;
    varlen.reserve(container.size());
    std::copy(std::cbegin(container), std::cend(container),
              std::back_inserter(varlen));
    return varlen;
}

template <typename Container,
          typename C = traits::value_t<Container>,
          typename R = Vector<VarLen<traits::value_t<C>>>>
inline auto
makeVarLens(Container&& container)
{
    R varlens;
    varlens.reserve(container.size());
    std::transform(std::cbegin(container), std::cend(container),
                   std::back_inserter(varlens), [](C const& c){
        return makeVarLen(c);
    });
    return varlens;
}

template <typename... Args>
inline auto
makeComp0D(Args&&... args) noexcept(false)
{
    return std::make_tuple(std::forward<Args>(args)...);
}

template <typename... Containers>
inline auto
makeComp(Containers&&... containersIn) noexcept(false)
{
    using Ttarget = Comp<traits::value_t<Containers>...>;

    Vector<Ttarget> tuples;

    // for iterating more easily over variadic arguments
    auto containers = std::make_tuple(&containersIn...);
    auto size = get<0>(containers)->size();
    tuples.resize(size);

    mpl::static_for<sizeof... (Containers)>([&](auto const idx){
        auto* container = get<idx>(containers);
        if (size != container->size())
        {
            throw InvalidArgumentError{
                GENH5_MAKE_EXECEPTION_STR_ID("makeComp")
                "Input containers have different number of elements (" +
                std::to_string(size) + " != " +
                std::to_string(container->size()) + ')'
            };
        }

        int i = 0;
        for (auto const& value : *container)
        {
            get<idx>(tuples[i++]) = value;
        }
    });

    return tuples;
}

/** UNPACK **/
template <typename T, size_t N,
          typename Container,
          traits::if_value_types_equal<Container, T> = true>
inline void
unpack(Array<T, N> const& array, Container& container)
{
    container.reserve(N);
    std::copy(std::cbegin(array), std::cend(array),
              std::back_inserter(container));
}

template <typename T, size_t N,
          typename Container,
          typename C = traits::value_t<Container>,
          traits::if_value_types_equal<C, T> = true>
inline void
unpack(Vector<Array<T, N>> const& arrays, Container& container)
{
    container.reserve(N);
    std::transform(std::cbegin(arrays), std::cend(arrays),
                   std::back_inserter(container), [](auto const& array){
        C c{};
        unpack(array, c);
        return c;
    });
}

template <typename T,
          typename Container,
          traits::if_value_types_equal<Container, T> = true>
inline void
unpack(VarLen<T> const& varlen, Container& container)
{
    container.reserve(varlen.size());
    std::copy(std::cbegin(varlen), std::cend(varlen),
              std::back_inserter(container));
}

template <typename T, typename Container,
          typename C = traits::value_t<Container>,
          traits::if_value_types_equal<C, T> = true>
inline void
unpack(Vector<VarLen<T>> const& varlens, Container& container)
{
    container.reserve(varlens.size());
    std::transform(std::cbegin(varlens), std::cend(varlens),
                   std::back_inserter(container), [](auto const& varlen){
        C c{};
        unpack(varlen, c);
        return c;
    });
}

template <typename... Ts>
inline void
unpack(Comp<Ts...> const& tuple, Ts&... valuesIn)
{
    // for iterating more easily over variadic arguments
    auto values = std::make_tuple(&valuesIn...);

    mpl::static_for<sizeof...(Ts)>([&](auto const idx){
        *get<idx>(values) = get<idx>(tuple);
    });
}

template <typename Tuple, typename... Containers>
inline void
unpack(Vector<Tuple> const& tuples, Containers&... containersIn)
{
    // for iterating more easily over variadic arguments
    auto containers = std::make_tuple(&containersIn...);

    mpl::static_for<sizeof... (Containers)>([&](auto const idx){
        get<idx>(containers)->reserve(tuples.size());
    });

    for (auto const& tuple : qAsConst(tuples))
    {
        mpl::static_for<traits::comp_size<Tuple>::value>([&](auto const idx){
            get<idx>(containers)->push_back(get<idx>(tuple));
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
        get<idx>(containers)->resize(nested.size());
    });

    for (int i = 0; i < nested.size(); ++i)
    {
        unpack(nested[i], containersIn[i]...);
    }
}

} // namespace GenH5

#endif // GENH5_UTILS_H
