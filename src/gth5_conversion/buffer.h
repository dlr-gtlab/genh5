/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 07.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_CONVERSION_BUFFER_H
#define GTH5_CONVERSION_BUFFER_H

#include "type.h"

#define GTH5_DECLARE_BUFFER_TYPE(TYPE, BUFFER) \
    template <> \
    struct GtH5::buffer_element<TYPE> : \
            GtH5::details::buffer_element_impl<TYPE, BUFFER> {};

namespace GtH5
{

/* BUFFER TYPE */
namespace details
{

    // For mapping types and buffered types
    template<typename Tsrc, typename Tbuffer = Tsrc>
    struct buffer_element_impl
    {
        using type = Tbuffer;
    };

} // namespace details

// forward declaration
template<typename ...Ts>
struct buffer_element;

// using declaration
template<typename ...Ts>
using buffer_element_t = typename buffer_element<Ts...>::type;

// single element
template<typename T>
struct buffer_element<T> :
        details::buffer_element_impl<T, T> {};

/* BUFFER */
namespace details
{

    // actual buffer to use (by default vector of elements)
    template <typename T>
    struct buffer_impl
    {
        using type = Vector<buffer_element_t<T>>;
    };

    // actual buffer for compound types
    template <typename... Ts>
    struct buffer_impl<Comp<Ts...>>
    {
        using type = buffer_element_t<Comp<Ts...>>;
    };

    // delegate buffer for array types
    template <typename T, size_t N>
    struct buffer_impl<Array<T, N>>
    {
        using type = typename buffer_impl<T>::type;
    };


} // namespace details

template <typename T>
using buffer_t = typename details::buffer_impl<T>::type;

namespace details
{
    template <typename T>
    struct hvl_buffer
    {
        /// hvl_t::p points to this
        Vector<conversion_t<T>> data;
        /// buffer for conversion
        buffer_t<T> buffer;
    };

} // namespace details

/* BUFFER TYPE SPECIALIZATIONS */
// array buffer type
template <typename T, size_t N>
struct buffer_element<Array<T, N>> :
        details::buffer_element_impl<Array<T, N>, buffer_element_t<T>> {};

// varlen buffer type
template <typename T>
struct buffer_element<VarLen<T>> :
        details::buffer_element_impl<VarLen<T>, details::hvl_buffer<T>> {};

// compound buffer type
template <typename ...Ts>
struct buffer_element<Comp<Ts...>>
{
    using type = mpl::reversed_comp_t<Comp<buffer_t<Ts>...>>;
};

} // namespace GtH5

#endif // GTH5_CONVERSION_BUFFER_H
