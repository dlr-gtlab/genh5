/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 17.5.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_STRINGVIEW_H
#define GENH5_STRINGVIEW_H

#include "genh5_typedefs.h"
#include "genh5_typetraits.h"

namespace GenH5
{

class StringView
{
public:

    using value_type     = String;
    using size_type      = typename value_type::size_type;

    using iterator       = typename value_type::iterator;
    using const_iterator = typename value_type::const_iterator;
    using reverse_iterator       = typename value_type::reverse_iterator;
    using const_reverse_iterator = typename value_type::const_reverse_iterator;

    /* constructors */
    StringView(value_type str) : m_string(std::move(str)) {}

    template <typename T,
              traits::if_convertible<decltype(std::declval<T>().toStdString()), String> = true>
    StringView(T const& str) : m_string(str.toStdString()) {}

    char const* data() const noexcept { return m_string.data(); }
    char const* c_str() const noexcept { return m_string.c_str(); }

//    value_type& get() { return m_string; }
    value_type const& get() const noexcept { return m_string; }

    size_type size() const noexcept { return m_string.size(); }

//    auto erase(const const_iterator where) noexcept { return m_string.erase(where); }
//    auto erase(const const_iterator first, const const_iterator last) noexcept { return m_string.erase(first, last); }

    /* iterators */
//    iterator begin() noexcept { return m_string.begin(); }
    const_iterator begin() const noexcept { return m_string.cbegin(); }
    const_iterator cbegin() const noexcept { return m_string.cbegin(); }
//    reverse_iterator rbegin() noexcept { return m_string.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return m_string.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return m_string.crbegin(); }

//    iterator end() noexcept { return m_string.end(); }
    const_iterator end() const noexcept { return m_string.cend(); }
    const_iterator cend() const noexcept { return m_string.cend(); }
//    reverse_iterator rend() noexcept { return m_string.rend(); }
    const_reverse_iterator rend() const noexcept { return m_string.rend(); }
    const_reverse_iterator crend() const noexcept { return m_string.crbegin(); }

    /* operators */
//    operator value_type&() { return get(); }
    operator value_type const&() const { return get(); }

    operator char const*() const { return data(); }

private:

    value_type m_string;
};

}

#endif // GENH5_STRINGVIEW_H
