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
            buffer.shrink_to_fit();
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
            buffer.reserve(buffer.size() + size);
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

} // namespace details

} // namespace GenH5

#endif // BUFFER_H
