/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 5.6.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */


#ifndef GENH5_BYTEARRAY_H
#define GENH5_BYTEARRAY_H

#include <genh5_typedefs.h>

namespace GenH5
{

/// A basic string class, designed to be used as a buffer for strings.
/// Allows moves only.
template <typename T = char>
class basic_byte_array
{

public:

    using container_type = Vector<T>;

    using value_type             = T;
    using iterator               = typename container_type::iterator;
    using const_iterator         = typename container_type::const_iterator;
    using reverse_iterator       = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;
    using size_type             = typename container_type::size_type;

    using pointer               = value_type*;
    using const_pointer         = value_type const*;
    using reference             = value_type&;
    using const_reference       = value_type const&;

    basic_byte_array() = default;
    basic_byte_array(GenH5::String const& s)
    {
        m_data.resize(s.size() +1 );
        std::copy(s.begin(), s.end(), std::begin(m_data));
    }
    basic_byte_array(T const* s)
    {
        if (!s) return;
        size_t len = strlen(s) + 1;
        m_data.resize(len);
        std::copy(s, s + len, std::begin(m_data));
    }

    basic_byte_array(basic_byte_array const&) = delete;
    basic_byte_array& operator=(basic_byte_array const&) = delete;
    basic_byte_array(basic_byte_array&& o) : m_data{std::move(o.m_data)} { }
    basic_byte_array& operator=(basic_byte_array&& o)
    {
        basic_byte_array tmp{std::move(o)};
        swap(tmp);
        return *this;
    };

    void push_back(GenH5::String const& s)
    {
        size_t oldSize = size();
        m_data.resize(size() + s.size() + 1);
        if (!empty()) erase(end() - 1);
        std::copy(s.begin(), s.end(), std::next(begin(), oldSize));
    }

    void push_back(T const* s)
    {
        if (!s) return;
        size_t oldSize = size();
        size_t len = strlen(s) + 1;
        m_data.resize(size() + len + 1);
        if (!empty()) erase(end() - 1);
        std::copy(s, s + len, std::next(begin(), oldSize));
    }

    void clear() { m_data.clear(); }
    size_type size() const noexcept { return empty() ? 0 : m_data.size() - 1; }
    bool empty() const noexcept { return m_data.empty(); }
    size_type capacity() const noexcept { return m_data.capacity(); }

    pointer data() noexcept { return m_data.data(); }
    const_pointer data() const noexcept { return m_data.data(); }

    reference front() { return m_data.front(); }
    const_reference front() const { return m_data.front(); }
    reference back() { return m_data.back(); }
    const_reference back() const { return m_data.back(); }

    void erase(const_iterator const& where) { m_data.erase(where); }
    void erase(const_iterator const& first,
               const_iterator const& last) { m_data.erase(first, last); }

    iterator begin() noexcept { return m_data.begin(); }
    const_iterator begin() const noexcept { return m_data.cbegin(); }
    const_iterator cbegin() const noexcept { return m_data.cbegin(); }
    reverse_iterator rbegin() noexcept { return m_data.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return m_data.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return m_data.crbegin(); }

    iterator end() noexcept { return m_data.end(); }
    const_iterator end() const noexcept { return m_data.cend(); }
    const_iterator cend() const noexcept { return m_data.cend(); }
    reverse_iterator rend() noexcept { return m_data.rend(); }
    const_reverse_iterator rend() const noexcept { return m_data.rend(); }
    const_reverse_iterator crend() const noexcept { return m_data.crbegin(); }

    void swap(basic_byte_array& o) noexcept
    {
        using std::swap;
        swap(m_data, o.m_data);
    }

    reference operator[](size_type i) { return m_data[i]; }
    const_reference operator[](size_type i) const { return m_data[i]; }

    reference at(size_type idx) {
        assert(idx < size());
        return operator[](idx);
    }
    const_reference at(size_type idx) const {
        assert(idx < size());
        return operator[](idx);
    }

    template <typename Arg>
    basic_byte_array& operator+=(Arg&& arg) {
        push_back(std::forward<Arg>(arg));
        return *this;
    }

private:

    container_type m_data;
};

// alias
using ByteArray = basic_byte_array<char>;

} // namespace GenH5

template <typename T>
void swap(GenH5::basic_byte_array<T>& a, GenH5::basic_byte_array<T>& b) noexcept { a.swap(b); }

#endif // GENH5_BYTEARRAY_H
