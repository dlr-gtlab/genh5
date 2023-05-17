/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 12.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef FIXEDSTRING0D_H
#define FIXEDSTRING0D_H

#include "genh5_data/base.h"
#include "genh5_data/buffer.h"

namespace GenH5
{

/** FIXED STRING 0D **/
class FixedString0D : public details::AbstractData<String>
{
    using T = String;

public:

    FixedString0D() = default;

    using compound_names = CompoundNames<0>;
    using value_type     = conversion_t<T>;
    using buffer_type    = T;

    // compound type not supported
    static DataType dataType(compound_names names) = delete;

    DataSpace dataSpace() const override { return DataSpace::Scalar; }

    DataType dataType() const override { return dataType(true); }

    DataType dataType(bool utf8) const
    {
        return DataType::string(stringSize(), utf8);
    }

    /** general constructors **/
    // buffer type
    // cppcheck-suppress noExplicitConstructor
    FixedString0D(buffer_type arg) :
        m_data{std::move(arg)}
    { }

    template <typename U,
             traits::if_convertible<decltype(std::declval<U>().toStdString()), T> = true>
    FixedString0D(U const& str) : m_data(str.toStdString()) {}

    /** conversion constructors **/
    // frwd ref for template type
    template <typename U, traits::if_not_convertible<U, T> = true>
    // cppcheck-suppress noExplicitConstructor
    FixedString0D(U&& arg)
    {
        using GenH5::convert; // ADL
        // buffer for conversions
        details::StaticBuffer<T> buffer;
        // convert to char* and back to string
        // cppcheck-suppress useInitializationList
        m_data = convertTo<T>(convert(std::forward<U>(arg), buffer));
    }

    /** deserialize **/
    // by value
    template <typename U = buffer_type>
    auto value() const
    {
        using GenH5::convertTo; // ADL
        return convertTo<U>(m_data.data());
    }

    void resize(size_type size) { m_data.resize(size + 1); }

    bool resize(DataSpace const& dspace, DataType const& dtype) override
    {
        if (dspace.selectionSize() > size() &&
            dtype.type() != H5T_STRING &&
            dtype.isVarString())
        {
            return false;
        }

        m_data.resize(static_cast<size_type>(dtype.size() + 1));
        return true;
    }

    // pointer for reading
    void* dataReadPtr() override
    {
        // *sigh* (there is no "char*" overload of "data()" prior c++17)
        return const_cast<void*>(static_cast<void const*>(m_data.data()));
    }

    // pointer for writing
    void const* dataWritePtr() const override { return data(); }

    /** implicit conversions **/
    operator char const*() const { return data(); }

    /** for accessing raw data **/
    buffer_type& raw() { return m_data; }
    buffer_type const& raw() const { return m_data; }

    size_type size() const override { return 1; }
    size_type stringSize() const { return m_data.size(); }

    /** STL **/
    char const* data() const { return m_data.data(); }

    /** Qt **/
    char const* constData() const { return data(); }

private:

    /// buffer holding data
    buffer_type m_data;
};

// somewhat hacky solution to forward Data<FixedString0D> to FixedString0D
namespace details
{

/** DATA VECTOR **/
template<typename... Ts>
struct data_helper;

template<>
struct data_helper<FixedString0D>
{
    using type = FixedString0D;
};

/** DATA 0D **/
template<typename... Ts>
struct data_0d_helper;

template<>
struct data_0d_helper<FixedString0D>
{
    using type = FixedString0D;
};

} // namespace details

template <typename Arg>
inline auto
makeFixedStr(Arg&& args)
{
    return FixedString0D{std::forward<Arg>(args)};
}

} // namespace GenH5

#endif // FIXEDSTRING0D_H
