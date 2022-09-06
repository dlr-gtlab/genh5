/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 17.08.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATA_COMMON0D_H
#define GENH5_DATA_COMMON0D_H

#include "genh5_conversion.h"
#include "genh5_dataspace.h"

#include "base.h"

namespace GenH5
{

/** DATA 0D **/
template<typename T>
class CommonData0D : public details::AbstractData<T>
{
    using base_class =  details::AbstractData<T>;

public:

    CommonData0D() = default;

    using compound_names = typename base_class::compound_names;
    using size_type      = typename base_class::size_type;

    using template_type         = T;
    using value_type            = conversion_t<T>;
    using buffer_element_type   = buffer_element_t<T>;
    using buffer_type           = buffer_t<T>;

    DataSpace dataSpace() const override
    {
        return DataSpace::Scalar;
    }

    /** general constructors **/
    // types names only
    explicit CommonData0D(compound_names names)
        : base_class{names}
    { }
    // value type
    explicit CommonData0D(value_type arg, compound_names names = {}) :
        m_data{std::move(arg)},
        base_class{names}
    { }

    /** conversion constructors **/
    // frwd ref for template type
    template <typename U, traits::if_types_equal<U, T> = true>
    explicit CommonData0D(U&& arg, compound_names names = {}) :
        base_class{names}
    {
        using GenH5::convert; // ADL
        m_data = convert(std::forward<U>(arg), m_buffer);
    }

    /** general assignments **/
    // value type
    CommonData0D& operator=(value_type data)
    {
        m_data = std::move(data);
        return *this;
    }

    /** conversion assignments **/
    // frwd ref for template type
    template <typename U, traits::if_types_equal<U, T> = true>
    CommonData0D& operator=(U&& arg)
    {
        using GenH5::convert; // ADL
        m_data = convert(std::forward<U>(arg), m_buffer);
        return *this;
    }

    /** deserialize **/
    // by value
    auto value() const
    {
        using GenH5::convertTo; // ADL
        return convertTo<T>(m_data);
    }

    // resize data
    bool resize(DataSpace const& dspace, DataType const& /*dtype*/) override
    {
        return dspace.selectionSize() <= size();
    }

    // pointer for reading
    void* dataReadPtr() override { return data(); }

    // pointer for writing
    void const* dataWritePtr() const override { return data(); }

    /** for accessing raw data **/
    operator value_type const&() const { return m_data; }
    operator value_type&() { return m_data; }

    value_type& raw() { return m_data; }
    value_type const& raw() const { return m_data; }

    size_type size() const override { return 1; }

    /** STL **/
    value_type* data() { return &m_data; }
    value_type const* data() const { return &m_data; }

    /** Qt **/
    value_type const* constData() const { return data(); }

protected:

    buffer_type m_buffer{};
    value_type m_data{};
};

namespace details
{

/** DATA 0D **/
template<typename... Ts>
struct data_0d_helper;

template<typename  T>
struct data_0d_helper<T>
{
    using type = CommonData0D<T>;
};

} // namespace details

template<typename... Ts>
using Data0D = typename details::data_0d_helper<Ts...>::type;

template <typename... Args>
inline auto
makeData0D(Args&&... args)
{
    return Data0D<traits::value_t<Args>...>{
        std::forward<Args>(args)...
    };
}

} // namespace GenH5

#endif // GENH5_DATA_COMMON0D_H
