/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 07.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_CONVERSION_BUFFER_H
#define GENH5_CONVERSION_BUFFER_H

#include "genh5_conversion/type.h"

#define GENH5_DECLARE_BUFFER_TYPE(TYPE, BUFFER) \
    template <> \
    struct GenH5::buffer_element<TYPE> : \
           GenH5::details::buffer_element_impl<TYPE, BUFFER> {};

namespace GenH5
{

/** BUFFER ELEMENT TYPE **/
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

/** BUFFER TYPE **/
namespace details
{

// actual buffer to use (by default vector of buffer elements)
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

template <typename T, size_t N>
struct buffer_impl<T[N]>
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

/** BUFFER ELEMENTTYPE SPECIALIZATIONS **/

// single element
template<typename T>
struct buffer_element<T> :
        details::buffer_element_impl<T, T> {};

// array buffer type
template <typename T, size_t N>
struct buffer_element<Array<T, N>> :
        details::buffer_element_impl<Array<T, N>, buffer_element_t<T>> {};

template <typename T, size_t N>
struct buffer_element<T[N]> :
        details::buffer_element_impl<T[N], buffer_element_t<T>> {};

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

} // namespace GenH5

#endif // GENH5_CONVERSION_BUFFER_H
