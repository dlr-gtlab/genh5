/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 20.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef BUFFER_H
#define BUFFER_H

#include "genh5_conversion/buffer.h"
#include "genh5_utils.h"

namespace GenH5
{

namespace details
{

template <typename T, typename Lambda>
void applyToBuffer(buffer_t<T>&, Lambda&&);

template <typename T>
class StaticBuffer
{
public:

    using buffer_element_type = buffer_element_t<T>;
    using buffer_type         = buffer_t<T>;
    using size_type           = typename conversion_container_t<T>::size_type;

#ifndef GENH5_NO_STATIC_BUFFER
    // incr ref count
    StaticBuffer() { ++m_ref; }
    StaticBuffer(StaticBuffer const&) { ++m_ref; }
    StaticBuffer(StaticBuffer&&) { ++m_ref; }

    // incr ref count not required
    StaticBuffer& operator=(StaticBuffer const&) { return *this; }
    StaticBuffer& operator=(StaticBuffer&&) { return *this; }

    // decr ref count -> clear elements
    ~StaticBuffer()
    {
        assert(m_ref > 0);
        if (--m_ref == 0)
        {
            clear();
        }
    }
#endif

    /**
     * @brief Clear the buffer
     */
    void clear()
    {
        applyToBuffer<T>(m_buffer, [](auto& buffer){
            buffer.clear();
            buffer.squeeze();
        });
    }

    /**
     * @brief Reserves the buffer by additional size elements.
     * @param size
     */
    void reserve(size_type size)
    {
#ifndef GENH5_NO_BUFFER_AUTORESERVE
        applyToBuffer<T>(m_buffer, [=](auto& buffer){
            buffer.reserve(static_cast<int>(buffer.size() + size));
        });
#endif
    }

    /// call operator as getter for acutal buffer
    buffer_type& operator()() { return m_buffer; }

    /// getter for acutal buffer
    buffer_type& get() { return m_buffer; }

    /// implicit conversion
    operator buffer_type&() { return m_buffer; }

private:

    /// shared buffer (probably not thread-safe)
#ifndef GENH5_NO_STATIC_BUFFER
    static
#endif
    buffer_type m_buffer;

    /// ref count for clearing
    static int m_ref;
};

#ifndef GENH5_NO_STATIC_BUFFER
/// static buffer init
template <typename T>
buffer_t<T> StaticBuffer<T>::m_buffer{};
#endif

/// static ref count init
template <typename T>
int StaticBuffer<T>::m_ref{};


/** APPLY TO BUFFER **/
template <typename...>
struct apply_to_buffer_impl;

// value_type == buffer_type
template <typename value_type>
struct apply_to_buffer_impl<value_type, Vector<value_type>>
{
    template <typename Lambda>
    static void apply(Vector<value_type>&, Lambda&&)
    {
        // NO-OP
    }
};

// value_type != buffer_type
template <typename value_type, typename buffer_type>
struct apply_to_buffer_impl<value_type, buffer_type>
{
    template <typename Lambda>
    static void apply(buffer_type& buffer, Lambda&& lambda)
    {
        lambda(buffer);
    }
};

// for each compound element
template <typename value_type, typename... Ts>
struct apply_to_buffer_impl<value_type, Comp<Ts...>>
{
    using buffer_type = Comp<Ts...>;

    template <typename Lambda>
    static void apply(buffer_type& buffer, Lambda&& lambda)
    {
        mpl::static_for<sizeof...(Ts)>([&](const auto idx)
        {
            using Tsrc = traits::comp_element_t<idx, value_type>;
            using Tbuffer = traits::comp_element_t<idx, buffer_type>;

            apply_to_buffer_impl<Tsrc, Tbuffer>::apply(
                        get<idx>(buffer), std::forward<Lambda>(lambda));
        });
    }
};

/**
 * @brief Applies a method (lambda) to the buffer
 * (for each buffer element in case of compound buffer)
 * @param buffer Buffer
 * @param lambda lambda to apply
 */
template <typename T, typename Lambda>
inline void
applyToBuffer(buffer_t<T>& buffer, Lambda&& lambda)
{
    apply_to_buffer_impl<traits::base_t<conversion_t<T>>, buffer_t<T>>
            ::apply(buffer, std::forward<Lambda>(lambda));
}

} // namespace details

} // namespace GenH5

#endif // BUFFER_H
