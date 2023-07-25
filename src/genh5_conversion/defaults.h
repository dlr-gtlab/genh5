/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 19.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_CONVERSION_DEFAULTS_H
#define GENH5_CONVERSION_DEFAULTS_H

#include "genh5_conversion/buffer.h"
#include "genh5_datatype.h"

/**
 * Checklist for supporting new types
 * 1. Register Conversion Type (either using implicit or explicit conversion)
 *    a) implicit: e.g. double -> double
 *    b) explciit: e.g. std::string -> char*
 * 3. Register HDF5 Data Type
 * 4. Register a Buffer Type if conversion requires buffering
 * 5. Register a Conversion Method if using explicit conversion
 * 6. Register a Reverse Conversion if implicit reverse conversion does not work
 */

#define GENH5_DECLARE_IMPLICIT_CONVERSION(TYPE) \
    /* buffered */ \
    inline GenH5::conversion_t<GenH5::traits::decay_crv_t<TYPE>> \
    convert(TYPE value, GenH5::buffer_t<TYPE>& /*...*/) { \
        return value; \
    } \
    /* plain */ \
    inline GenH5::conversion_t<GenH5::traits::decay_crv_t<TYPE>> \
    convert(TYPE value) { \
        return value; \
    } \

#define GENH5_DECLARE_CONVERSION(TYPE_FROM, VALUE_NAME, BUFFER_NAME) \
    inline GenH5::conversion_t<GenH5::traits::decay_crv_t<TYPE_FROM>> \
    convert(TYPE_FROM VALUE_NAME,  \
            GenH5::buffer_t<GenH5::traits::decay_crv_t<TYPE_FROM>>& BUFFER_NAME)

#define GENH5_DECLARE_REVERSE_CONVERSION(TYPE_TO, TYPE_FROM, VALUE_NAME) \
    template <> \
    inline auto \
    GenH5::convertTo<TYPE_TO,TYPE_FROM>(TYPE_FROM VALUE_NAME)

#define GENH5_DECLARE_DATATYPE(NATIVE_TYPE, H5_TYPE) \
    template <> \
    struct GenH5::details::datatype_impl<NATIVE_TYPE> { \
        datatype_impl(CompoundNames<0> = {}) {} \
        operator GenH5::DataType() const { return H5_TYPE; } \
    };

#define GENH5_DECLARE_DATATYPE_IMPL(NATIVE_TYPE) \
    template <> \
    struct GenH5::details::datatype_impl<NATIVE_TYPE> { \
        datatype_impl(CompoundNames<0> = {}) {} \
        operator GenH5::DataType() const; \
    }; \
    inline \
    GenH5::details::datatype_impl<NATIVE_TYPE>::operator GenH5::DataType() const

namespace GenH5
{

/** GENERIC REVERSE CONVERSION **/
template<typename Ttarget, typename Tfrom = Ttarget&&>
inline auto
convertTo(Tfrom value)
{
    // generic reverse conversion
    return static_cast<Ttarget>(std::forward<Tfrom>(value));
}

/** default conversions **/
GENH5_DECLARE_IMPLICIT_CONVERSION(bool);

GENH5_DECLARE_IMPLICIT_CONVERSION(char);
GENH5_DECLARE_IMPLICIT_CONVERSION(char*);
GENH5_DECLARE_IMPLICIT_CONVERSION(char const*);

GENH5_DECLARE_IMPLICIT_CONVERSION(int);
GENH5_DECLARE_IMPLICIT_CONVERSION(long int);
GENH5_DECLARE_IMPLICIT_CONVERSION(long long int);
GENH5_DECLARE_IMPLICIT_CONVERSION(unsigned int);
GENH5_DECLARE_IMPLICIT_CONVERSION(unsigned long int);
GENH5_DECLARE_IMPLICIT_CONVERSION(unsigned long long int);

GENH5_DECLARE_IMPLICIT_CONVERSION(float);
GENH5_DECLARE_IMPLICIT_CONVERSION(double);

/** datatype mapping **/

namespace details
{

// invalid, cannot convert to datatype
template <typename ...Ts>
struct datatype_impl
{
    // T must have an datatype associated!
};

// specialization for array types
template <typename T, size_t N>
struct datatype_impl<Array<T, N>>
{
    datatype_impl(CompoundNames<0> = {}) {}
    operator DataType() const
    {
        return DataType::array(datatype_impl<T>(), N);
    }
};

template <typename T, size_t N>
struct datatype_impl<T[N]>
{
    datatype_impl(CompoundNames<0> = {}) {}
    operator DataType() const
    {
        return datatype_impl<Array<T, N>>();
    }
};

// specialization for varlen types
template <typename T>
struct datatype_impl<VarLen<T>>
{
    datatype_impl(CompoundNames<0> = {}) {}
    operator DataType() const
    {
        return DataType::varLen(datatype_impl<T>());
    }
};

// specialization for compound types
template <typename... Ts>
struct datatype_impl<Comp<Ts...>>
{
    using Compound  = conversion_t<Comp<Ts...>>;
    using TypeNames = CompoundNames<sizeof...(Ts)>;

    datatype_impl(TypeNames memberNames = {})
        : m_typeNames(std::move(memberNames))
    {
        generateDefaultNames();
    }

    operator DataType() const
    {
        CompoundMembers members;
        members.reserve(sizeof...(Ts));

        mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            members.push_back({
                m_typeNames.at(traits::comp_size<Compound>()-1-idx),
                offset<idx>(),
                datatype_impl<traits::rcomp_element_t<idx, Comp<Ts...>>>()
            });
        });
        // members are flipped, not necessary for compound type creation,
        // but makes debugging easier
        std::reverse(std::begin(members), std::end(members));

        return DataType::compound(sizeof(Compound), members);
    }

private:
    /// compound member for calculating offset of an element
    static Compound m_t;
    /// names for compound members
    TypeNames m_typeNames{};

    /// offset of tuple element
    template<size_t Idx>
    size_t offset() const
    {
        return reinterpret_cast<size_t>(&get<Idx>(m_t)) -
               reinterpret_cast<size_t>(&m_t);
    }

    void generateDefaultNames()
    {
        for (size_t i = 0; i < sizeof...(Ts); ++i)
        {
            if (m_typeNames[i].empty())
            {
                // String::number ambiguous for size_t
                m_typeNames[i] = String{"type_"} + std::to_string(i);
            }
        }
    }
};

// static compound_type init
template <typename... Ts>
conversion_t<Comp<Ts...>> datatype_impl<Comp<Ts...>>::m_t{};

} // namespace details

// compound types
template<typename T1, typename T2, typename... Tother>
inline DataType
dataType(CompoundNames<sizeof...(Tother) + 2> memberNames = {}) noexcept(false)
{
    using T = Comp<T1, T2, Tother...>;
    return details::datatype_impl<T>{std::move(memberNames)};
}

// compound names
template<typename T>
inline DataType
dataType(CompoundNames<traits::comp_size<T>::value> memberNames) noexcept(false)
{
    return details::datatype_impl<T>{std::move(memberNames)};
}

template<typename T>
inline DataType
dataType() noexcept(false)
{
    return details::datatype_impl<T>{};
}

} // namespace GenH5

/** default datatypes **/
GENH5_DECLARE_DATATYPE(bool, DataType::Bool);

GENH5_DECLARE_DATATYPE(char, DataType::Char);

GENH5_DECLARE_DATATYPE(char*, DataType::VarString);
GENH5_DECLARE_DATATYPE(char const*, DataType::VarString);

GENH5_DECLARE_DATATYPE(int, DataType::Int);
GENH5_DECLARE_DATATYPE(long int, DataType::Long);
GENH5_DECLARE_DATATYPE(long long int, DataType::LLong);
GENH5_DECLARE_DATATYPE(unsigned int, DataType::UInt);
GENH5_DECLARE_DATATYPE(unsigned long int, DataType::ULong);
GENH5_DECLARE_DATATYPE(unsigned long long int, DataType::ULLong);

GENH5_DECLARE_DATATYPE(float, DataType::Float);
GENH5_DECLARE_DATATYPE(double, DataType::Double);

// fixed string
template <size_t N>
struct GenH5::details::datatype_impl<GenH5::Array<char, N>> {
    datatype_impl(CompoundNames<0> = {}) {}
    operator GenH5::DataType() const { return GenH5::DataType::string(N); }
};

#endif // GENH5_CONVERSION_DEFAULTS_H
