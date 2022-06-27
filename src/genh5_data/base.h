/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 13.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATA_BASE_H
#define GENH5_DATA_BASE_H

#include "genh5_conversion.h"
#include "genh5_dataspace.h"

namespace GenH5
{

namespace details
{

template <typename T>
void reserveBuffer(buffer_t<T>& buffer, size_t size);

} // namsepace details

template<typename T>
class AbstractData
{
public:

    AbstractData() = default;
    /*virtual*/ ~AbstractData() = default;
    AbstractData(AbstractData const&) = default;
    AbstractData(AbstractData&&) = default;
    AbstractData& operator=(AbstractData const&) = default;
    AbstractData& operator=(AbstractData&&) = default;

    using template_type         = T;
    using value_type            = conversion_t<T>;
    using buffer_element_type   = buffer_element_t<T>;
    using buffer_type           = buffer_t<T>;
    using container_type        = Vector<value_type>;

    // only necessary for when creating compound types
    using compound_names = CompoundNames<traits::comp_size<T>::value>;

    using iterator              = typename container_type::iterator;
    using const_iterator        = typename container_type::const_iterator;

    using pointer               = value_type*;
    using const_pointer         = value_type const*;
    using reference             = value_type&;
    using const_reference       = value_type const&;
    using size_type             = typename container_type::size_type;

    static DataType dataType(compound_names names) noexcept(false)
    {
        using GenH5::dataType;
        return dataType<T>(names);
    }
    DataType dataType() const noexcept(false)
    {
        using GenH5::dataType;
        return dataType<T>(m_typeNames);
    }
    DataSpace dataSpace() const noexcept(false)
    {
        return DataSpace::linear(size());
    }

    // constructors
    explicit AbstractData(compound_names names)
        : m_typeNames{names}
    { }
    explicit AbstractData(container_type data, compound_names names = {}) :
        m_data{std::move(data)},
        m_typeNames{names}
    { }
    explicit AbstractData(value_type arg, compound_names names = {}) :
        m_data{std::move(arg)},
        m_typeNames{names}
    { }
    explicit AbstractData(std::initializer_list<value_type> args,
                          compound_names names = {}) :
        m_data{std::move(args)},
        m_typeNames{names}
    { }
    // conversion constructors
    template <typename Container,
              traits::if_types_differ<Container, T> = true,
              traits::if_types_differ<Container, container_type> = true,
              traits::if_has_value_type<Container, T> = true>
    explicit AbstractData(Container&& c, compound_names names = {})
        : m_typeNames{names}
    {
        push_back(std::forward<Container>(c));
    }
    template <typename U, traits::if_types_equal<U, T> = true>
    explicit AbstractData(U&& arg, compound_names names = {}) :
        m_typeNames{names}
    {
        push_back(std::forward<U>(arg));
    }
    template <typename U, traits::if_types_equal<U, T> = true>
    explicit AbstractData(std::initializer_list<U>&& args,
                          compound_names names ={}) :
        m_typeNames{names}
    {
        push_back(std::forward<std::initializer_list<U>>(args));
    }

    // assignments
    AbstractData& operator=(container_type data)
    {
        m_data = std::move(data);
        return *this;
    }
    AbstractData& operator=(value_type data)
    {
        m_data.clear();
        push_back(std::move(data));
        return *this;
    }
    // conversion assignments
    template <typename Container,
              traits::if_types_differ<Container, T> = true,
              traits::if_types_differ<Container, container_type> = true,
              traits::if_has_value_type<Container, T> = true>
    AbstractData& operator=(Container&& c)
    {
        m_data.clear();
        push_back(std::forward<Container>(c));
        return *this;
    }
    template <typename U, traits::if_types_equal<U, T> = true>
    AbstractData& operator=(U&& arg)
    {
        m_data.clear();
        push_back(std::forward<U>(arg));
        return *this;
    }

    // push_back
    void push_back(container_type data)
    {
        m_data.append(std::move(data));
    }
    void push_back(value_type data)
    {
        m_data.push_back(std::move(data));
    }
    // conversion push_back
    template <typename Container,
              traits::if_types_differ<Container, T> = true,
              traits::if_types_differ<Container, container_type> = true,
              traits::if_has_value_type<Container, T> = true>
    void push_back(Container&& c)
    {
        using GenH5::convert; // ADL
        details::reserveBuffer<T>(m_buffer, c.size());
        m_data.reserve(c.size());
        for (auto const& value : qAsConst(c))
        {
            m_data.push_back(convert(value, m_buffer));
        }
    }
    template <typename U, traits::if_types_equal<U, T> = true>
    void push_back(U&& arg)
    {
        using GenH5::convert; // ADL
        m_data.push_back(convert(std::forward<U>(arg), m_buffer));
    }

    // deserialize
    T deserializeIdx(size_type idx) const
    {
        using GenH5::convertTo; // ADL
        return convertTo<T>(m_data[idx]);
    }
    template <typename Container = Vector<T>>
    Container deserialize() const
    {
        using GenH5::convertTo; // ADL
        Container c;
        c.reserve(size());
        for (auto const& value : qAsConst(m_data))
        {
            c.push_back(convertTo<T>(value));
        }
        return c;
    }
    template <typename Container, traits::if_has_value_type<Container, T>>
    void deserialize(Container& c) const
    {
        using GenH5::convertTo; // ADL
        c.reserve(size());
        for (auto const& value : qAsConst(m_data))
        {
            c.push_back(convertTo<T>(value));
        }
    }

    // access names used when generating a compound type
    compound_names const& typeNames() const
    {
        return m_typeNames;
    }
    // set compound type names
    void setTypeNames(compound_names names)
    {
        m_typeNames = std::move(names);
    }
    template <size_t N = traits::comp_size<T>::value>
    void setTypeNames(DataType const& dtype)
    {
        m_typeNames = getTypeNames<N>(dtype);
    }

    // index operator
    reference operator[](size_type i) { return m_data[i]; }
    const_reference operator[](size_type i) const { return m_data[i]; }

    // for accessing underlying container type
    operator container_type const&() const { return m_data; }
    operator container_type&() { return m_data; }
    container_type const& c() const { return m_data; }
    container_type& c() { return m_data; }

    /** STL **/
    void clear() { m_data.clear(); }
    bool empty() const { return m_data.empty(); }
    size_type size() const { return m_data.size(); }
    size_type capacity() const { return m_data.capacity(); }

    void reserve(size_type len) { return m_data.reserve(len); }
    void resize(size_type len) { return m_data.resize(len); }

    value_type* data() { return m_data.data(); }
    value_type const* data() const { return m_data.data(); }

    reference front() { return m_data.front(); }
    const_reference front() const { return m_data.front(); }
    reference back() { return m_data.back(); }
    const_reference back() const { return m_data.back(); }

    iterator begin() { return m_data.begin(); }
    const_iterator begin() const noexcept { return m_data.cbegin(); }
    iterator rbegin() noexcept { return m_data.rbegin(); }
    const_iterator rbegin() const noexcept { return m_data.rbegin(); }
    const_iterator cbegin() const noexcept { return m_data.cbegin(); }
    const_iterator crbegin() const noexcept { return m_data.crbegin(); }

    iterator end() { return m_data.end(); }
    const_iterator end() const noexcept { return m_data.cend(); }
    iterator rend() noexcept { return m_data.rend(); }
    const_iterator rend() const noexcept { return m_data.rend(); }
    const_iterator cend() const noexcept { return m_data.cend(); }
    const_iterator crend() const noexcept { return m_data.crbegin(); }

    reference at(size_type i) { return m_data[i]; }
    const_reference at(size_type i) const { return m_data.at(i); }

    /** Qt **/
    bool isEmpty() const { return empty(); }
    size_type length() const { return m_data.length(); }

    value_type const* constData() const { return data(); }

    reference first() { return m_data.first(); }
    const_reference first() const { return m_data.first(); }
    reference last() { return m_data.last(); }
    const_reference last() const { return m_data.last(); }

    template <typename U>
    void append(U&& arg) { push_back(std::forward<U>(arg)); }

    container_type mid(int pos, int len = -1) const
    {
        return m_data.mid(pos, len);
    }

protected:

    buffer_type m_buffer{};
    container_type m_data{};
    compound_names m_typeNames{};
};

namespace details
{

template <typename...>
struct reserve_buffer_impl;

template <typename value_type, typename buffer_type>
struct reserve_buffer_impl<value_type, buffer_type>
{
    static void reserve(buffer_type& buffer, size_t size)
    {
        if // if value_type == buffer_type no buffering required
#if __cplusplus >= 201703L // check for C++17
        constexpr
#endif
        (!std::is_same<value_type, traits::conv_base_t<buffer_type>>::value)
        {
            buffer.reserve(size);
        }
    }
};

template <typename value_type, typename... Ts>
struct reserve_buffer_impl<value_type, Comp<Ts...>>
{
    using buffer_type = Comp<Ts...>;

    static void reserve(buffer_type& buffer, size_t size)
    {
        mpl::static_for<sizeof...(Ts)>([&](const auto idx)
        {
            using Tsrc = std::tuple_element_t<idx, value_type>;
            using Tbuffer = std::tuple_element_t<idx, buffer_type>;

            reserve_buffer_impl<traits::conv_base_t<Tsrc>,
                                Tbuffer>::reserve(std::get<idx>(buffer), size);
        });
    }
};

template <typename T>
inline void
reserveBuffer(buffer_t<T>& buffer, size_t size)
{
#ifndef GENH5_NO_BUFFER_PRE_RESERVING
    details::reserve_buffer_impl<traits::conv_base_t<conversion_t<T>>,
                                 buffer_t<T>>::reserve(buffer, size);
#endif
}

} // namespace details

} // namespace GenH5

#endif // GENH5_DATA_BASE_H
