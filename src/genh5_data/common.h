/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 17.08.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATA_COMMON_H
#define GENH5_DATA_COMMON_H

#include "genh5_optional.h"

#include "genh5_data/base.h"
#include "genh5_data/buffer.h"

namespace GenH5
{

/** DATA VECTOR **/
template<typename T>
class CommonData : public details::AbstractData<T>
{
    using base_class =  details::AbstractData<T>;

public:

    CommonData() = default;

    using compound_names = typename base_class::compound_names;

    using value_type            = conversion_t<T>;
    using buffer_element_type   = buffer_element_t<T>;
    using buffer_type           = buffer_t<T>;
    using container_type        = conversion_container_t<T>;

    using iterator              = typename container_type::iterator;
    using const_iterator        = typename container_type::const_iterator;

    using pointer               = value_type*;
    using const_pointer         = value_type const*;
    using reference             = value_type&;
    using const_reference       = value_type const&;
    using size_type             = typename container_type::size_type;

    /** general constructors **/
    // types names only
    explicit CommonData(compound_names names)
        : base_class{names}
    { }
    // container type
    // cppcheck-suppress noExplicitConstructor
    CommonData(container_type data) :
        m_data{std::move(data)}
    { }
    // value type
    // cppcheck-suppress noExplicitConstructor
    CommonData(value_type arg) :
        m_data{std::move(arg)}
    { }
    // value type init list
    template<typename U,
             traits::if_types_equal<U, value_type>,
             traits::if_types_differ<U, T>>
    // cppcheck-suppress noExplicitConstructor
    CommonData(std::initializer_list<U> args) :
        m_data{std::move(args)}
    { }

    /** conversion constructors **/
    // arbitrary container types
    template <typename Container,
              traits::if_value_types_equal<Container, T> = true>
    // cppcheck-suppress noExplicitConstructor
    CommonData(Container&& c)
    {
        push_back(std::forward<Container>(c));
    }
    // frwd ref for template type
    template <typename U, traits::if_types_equal<U, T> = true>
    // cppcheck-suppress noExplicitConstructor
    CommonData(U&& arg)
    {
        push_back(std::forward<U>(arg));
    }
    // template type init list
    // cppcheck-suppress noExplicitConstructor
    CommonData(std::initializer_list<traits::convert_to_t<T>> args)
    {
        push_back(std::move(args));
    }

    /** general assignments **/
    // container type
    CommonData& operator=(container_type data)
    {
        m_data = std::move(data);
        return *this;
    }
    // value type
    CommonData& operator=(value_type data)
    {
        m_data.clear();
        push_back(std::move(data));
        return *this;
    }

    /** conversion assignments **/
    // arbitrary container types
    template <typename Container,
              traits::if_value_types_equal<Container, T> = true>
    CommonData& operator=(Container&& c)
    {
        m_data.clear();
        push_back(std::forward<Container>(c));
        return *this;
    }
    // frwd ref for template type
    template <typename U, traits::if_types_equal<U, T> = true>
    CommonData& operator=(U&& arg)
    {
        m_data.clear();
        push_back(std::forward<U>(arg));
        return *this;
    }

    /** push_back **/
    // container type
    void push_back(container_type data)
    {
        m_data.append(std::move(data));
    }
    // value type
    void push_back(value_type data)
    {
        m_data.push_back(std::move(data));
    }
    /** conversion push_back **/
    // arbitrary container types
    template <typename Container,
              traits::if_value_types_equal<Container, T> = true>
    void push_back(Container&& c)
    {
        using GenH5::convert; // ADL
        auto size = static_cast<size_type>(c.size());
        m_buffer.reserve(size);
        m_data.reserve(size);
        std::transform(std::cbegin(c), std::cend(c),
                       std::back_inserter(m_data), [&](auto const& value){
            return convert(value, m_buffer);
        });
    }
    // frwd ref for template type
    template <typename U, traits::if_types_equal<U, T> = true>
    void push_back(U&& arg)
    {
        using GenH5::convert; // ADL
        m_data.push_back(convert(std::forward<U>(arg), m_buffer));
    }

    /** set value **/
    void setValue(size_type i, value_type value)
    {
        assert(m_data.size() > i);
        m_data[i] = std::move(value);
    }

    template<typename U,
             traits::if_types_differ<U, value_type> = true,
             traits::if_types_equal<conversion_t<traits::decay_crv_t<U>>,
                                    value_type> = true>
    void setValue(size_type i, U&& value)
    {
        using GenH5::convert; // ADL
        setValue(i, convert(std::forward<U>(value), m_buffer));
    }

    /**
     * @brief Getter for value at idx.
     * @param idx index
     * @return value at idx
     */
    template <typename U = traits::convert_to_t<T>>
    auto value(size_type idx) const
    {
        assert(idx < size());
        using GenH5::convertTo; // ADL
        return convertTo<U>(m_data[idx]);
    }

    /**
     * @brief Overload for 2D access. Data Dimensions must have been set.
     * @param idxA 1fst index
     * @param idxB 2nd index
     * @return value at idx
     */
    template <typename U = traits::convert_to_t<T>>
    auto value(hsize_t idxA, hsize_t idxB) const
    {
        return value<U>(idx(m_dims, {idxA, idxB}));
    }

    /**
     * @brief Overload for ND access. Data Dimensions must have been set.
     * @param idxs list of indicies
     * @return value at idx
     */
    template <typename U = T>
    auto value(Vector<hsize_t> const& idxs) const
    {
        return value<U>(idx(m_dims, idxs));
    }

    /** values **/
    template <typename Container = Vector<traits::convert_to_t<T>>>
    auto values() const
    {
        Container c;
        unpack(c);
        return c;
    }

    /** dataspace  **/
    DataSpace dataSpace() const override
    {
        return m_dims.isDefault() ? DataSpace::linear(size()) :
                                    DataSpace{m_dims};
    }

    /** dimensions **/
    Dimensions const& dimensions() const
    {
        return m_dims;
    }

    void setDimensions(Dimensions dims) noexcept(false)
    {
        if (prod<size_type>(dims) > size())
        {
            throw InvalidArgumentError{
                GENH5_MAKE_EXECEPTION_STR()
                "Dimension size does not match data size (" +
                std::to_string(prod<size_type>(dims)) + " vs. " +
                std::to_string(size()) + ')'
            };
        }
        m_dims = std::move(dims);
    }

    void clearDimensions()
    {
        m_dims.clear();
    }

    /** unpack **/
    template <typename Container>
    void unpack(Container& c) const
    {
        using GenH5::convertTo; // ADL
        c.reserve(size());
        std::transform(std::cbegin(m_data), std::cend(m_data),
                       std::back_inserter(c), [](auto const& value){
            return convertTo<traits::value_t<Container>>(value);
        });
    }

    /** resize **/
    void resize(size_type len) { return m_data.resize(len); }

    bool resize(DataSpace const& dspace, DataType const& dtype) override
    {
        auto selection = static_cast<size_type>(dspace.selectionSize());
        auto dataSize = selection;

#ifndef GENH5_NO_DATA_AUTORESIZE
        auto ourDtype = this->dataType();

        // convenience for reading array types
        // e.g. Reading "Data<Array<int, 5>>" using "Data<int>"
        if (dtype.isArray() && dtype.superType() == ourDtype)
        {
            dataSize *= prod<size_type>(dtype.arrayDimensions());
        }
        // convenience for reading a compound array type
        // e.g. Reading "CompData<Array<int, 5>>" using "Data<int>"
        else if (dtype.isCompound())
        {
            auto dmembers = dtype.compoundMembers();
            if (dmembers.size() == 1 &&
                dmembers.front().type.isArray() &&
                dmembers.front().type.superType() == ourDtype)
            {
                dataSize *= prod<size_type>(dmembers[0].type.arrayDimensions());
            }
        }
#endif

        // resize data
        m_data.resize(dataSize);

        // set data dimensions from dataspace
        if (dspace.size() == selection)
        {
            setDimensions(dspace.dimensions());
        }

#ifndef GENH5_NO_DATA_AUTORESIZE
        // check if we have autoresized our data
        if (dataSize != selection)
        {
            // make sure our memory size is actually big enough
            return selection * dtype.size() <= size() * ourDtype.size();
        }
#endif

        // else check if we have alocated enough elements
        return selection <= size();
    }

    /** split data into sub ranges of size n **/
    template <typename Container = Vector<Vector<traits::convert_to_t<T>>>>
    auto split(size_type n) const noexcept(false)
    {
        using GenH5::convertTo; // ADL
        if (size() % n != 0)
        {
            throw InvalidArgumentError{
                GENH5_MAKE_EXECEPTION_STR()
                "Failed to split data into equally sized lists (" +
                std::to_string(size()) + " % " + std::to_string(n) + " != 0)"
            };
        }
        size_type length = size() / n;
        Container c;
        c.reserve(length);
        for (size_type i = 0; i < length; ++i)
        {
            using Tvec = traits::value_t<Container>;
            Tvec vec;
            vec.reserve(n);
            std::transform(cbegin() + i * n, cbegin() + (i + 1) * n,
                           std::back_inserter(vec), [&](auto const& value){
                return convertTo<traits::value_t<Tvec>>(value);
            });
            c.push_back(std::move(vec));
        }
        return c;
    }

    // pointer for reading
    void* dataReadPtr() override { return data(); }

    // pointer for writing
    void const* dataWritePtr() const override { return data(); }

    /** index operator **/
    // 1D
    reference operator[](size_type i) { return m_data[i]; }
    const_reference operator[](size_type i) const { return m_data[i]; }
    // ND
    reference operator[](Vector<hsize_t> const& idxs) {
        return operator[](idx(m_dims, idxs));
    }
    const_reference operator[](Vector<hsize_t> const& idxs) const {
        return operator[](idx(m_dims, idxs));
    }

    /** implicit conversions **/
    operator Vector<traits::convert_to_t<T>>() const { return values(); }

    operator container_type const&() const { return m_data; }
    operator container_type&() { return m_data; }

    /** for accessing raw data **/
    container_type const& raw() const { return m_data; }
    container_type& raw() { return m_data; }

    /** STL **/
    void clear() { m_data.clear(); m_dims.clear(); }
    bool empty() const { return m_data.empty(); }
    size_type size() const override { return m_data.size(); }
    size_type capacity() const { return m_data.capacity(); }

    void reserve(size_type len) { return m_data.reserve(len); }

    value_type* data() { return m_data.data(); }
    value_type const* data() const { return m_data.data(); }

    reference front() { return m_data.front(); }
    const_reference front() const { return m_data.front(); }
    reference back() { return m_data.back(); }
    const_reference back() const { return m_data.back(); }

    void remove(size_type i, uint n = 1) { m_data.remove(i, n); }

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

    // 1D
    reference at(size_type idx) {
        assert(idx < size());
        return operator[](idx);
    }
    const_reference at(size_type idx) const {
        assert(idx < size());
        return operator[](idx);
    }
    // 2D
    reference at(hsize_t idxA, hsize_t idxB) {
        return at(idx(m_dims, {idxA, idxB}));
    }
    const_reference at(hsize_t idxA, hsize_t idxB) const {
        return at(idx(m_dims, {idxA, idxB}));
    }
    // ND
    reference at(Vector<hsize_t> const& idxs) {
        return at(idx(m_dims, idxs));
    }
    const_reference at(Vector<hsize_t> const& idxs) const {
        return at(idx(m_dims, idxs));
    }

    /** Qt **/
    bool isEmpty() const { return empty(); }
    size_type length() const { return size(); }

    value_type const* constData() const { return data(); }

    reference first() { return front(); }
    const_reference first() const { return front(); }
    reference last() { return back(); }
    const_reference last() const { return back(); }

    /** append **/
    // frwd arguments to push_back
    template <typename U>
    void append(U&& arg) { push_back(std::forward<U>(arg)); }

    container_type mid(int pos, int len = -1) const
    {
        return m_data.mid(pos, len);
    }

protected:

    // actual data container
    container_type m_data{};
    /// data dimensions
    Optional<Dimensions> m_dims{};
    /// buffer
    details::StaticBuffer<T> m_buffer{};
};

namespace details
{

/** DATA VECTOR **/
template<typename... Ts>
struct data_helper;

template<typename T>
struct data_helper<T>
{
    using type = CommonData<T>;
};

} // namespace details

template<typename... Ts>
using Data = typename details::data_helper<Ts...>::type;

template <typename... Containers>
inline auto
makeData(Containers&&... containers)
{
    return Data<traits::value_t<Containers>...>{
        std::forward<Containers>(containers)...
    };
}

} // namespace GenH5

#endif // GENH5_DATA_COMMON_H
