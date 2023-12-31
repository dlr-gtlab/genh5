/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATATYPE_H
#define GENH5_DATATYPE_H

#include "genh5_idcomponent.h"
#include "genh5_object.h"
#include "genh5_conversion/type.h"
#include "genh5_typetraits.h"
#include "genh5_version.h"
#include "genh5_utils.h"

namespace GenH5
{
// forward decl
struct CompoundMember;

using CompoundMembers = Vector<CompoundMember>;

/**
 * @brief The DataType class
 */
class GENH5_EXPORT DataType : public Object
{
public:

    using Type = H5T_class_t;

    static DataType const& Bool;
    static DataType const& Char;

    static DataType const& Int;
    static DataType const& Long;
    static DataType const& LLong;
    static DataType const& UInt;
    static DataType const& ULong;
    static DataType const& ULLong;

    static DataType const& Float;
    static DataType const& Double;

    static DataType const& VarString;

    static DataType const& Version;

    /// will create an string type with utf8 enabled by default
    static DataType string(size_t size, bool useUtf8 = true) noexcept(false);

    static DataType array(DataType const& type,
                          Dimensions const& dims) noexcept(false);
    static DataType array(DataType const& type, hsize_t length) noexcept(false);

    static DataType varLen(DataType const& type) noexcept(false);

    static DataType compound(size_t dataSize,
                             CompoundMembers const& members) noexcept(false);

    /// Instantiates a new Datatype and assigns the id without incrementing it
    static DataType fromId(hid_t id) noexcept;

    DataType();
    explicit DataType(hid_t id);

    /**
     * @brief size
     * @return size of a datatype in bytes.
     */
    size_t size() const noexcept;

    /**
     * @brief type of this datatype (compound, array etc)
     * @return type
     */
    Type type() const noexcept;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief whether this datatype is of an int type.
     * @return true if is array type
     */
    bool isInt() const noexcept;

    /**
     * @brief whether this datatype is of an floating point type.
     * @return true if is array type
     */
    bool isFloat() const noexcept;

    /**
     * @brief  whether this datatype is a variable length string.
     * @return true if is var string
     */
    bool isString() const noexcept;

    /**
     * @brief whether this datatype is a fixed sized array.
     * @return true if is array type
     */
    bool isArray() const noexcept;

    /**
     * @brief whether this datatype is a fixed sized array.
     * @return true if is compound type
     */
    bool isCompound() const noexcept;

    /**
     * @brief  whether this datatype is a variable length datatype.
     * Will return false if type is var string.
     * @return true if is var len.
     */
    bool isVarLen() const noexcept;

    /**
     * @brief  whether this datatype is a variable length string.
     * @return true if is var string
     */
    bool isVarString() const noexcept;

    /**
     * @brief the dimensions of the fixed sized array datatype.
     * @return array dimensions. empty if not an array or process failed
     */
    Dimensions arrayDimensions() const noexcept;

    /**
     * @brief Returns the compound members of the compound datatype.
     * @return members. Empty if not an compound type
     */
    CompoundMembers compoundMembers() const noexcept(false);

    /**
     * @brief Returns the super datatype of this type. Used for accessing the
     * underlying type of an array or a varlen type.
     * @return super datatype
     */
    DataType superType() const noexcept(false);

    /// swaps all members
    void swap(DataType& other) noexcept;

protected:

    /// datatype id
    IdComponent<H5I_DATATYPE> m_id;
};

struct CompoundMember
{
    String name;
    size_t offset;
    DataType type;
};

/**
 * @brief Return the type names
 * @param dtype Datatype to extract compound members from
 */
template <size_t N>
inline CompoundNames<N>
getTypeNames(DataType const& dtype) noexcept(false)
{
    auto const members = dtype.compoundMembers();

    CompoundNames<N> names{};

    ssize_t offset = members.size() - N;
    if (offset >= 0)
    {
        std::transform(std::begin(members), std::end(members) - offset,
                       std::begin(names), [](auto const member){
            return member.name;
        });
    }
    return names;
}

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
        return GenH5::DataType::array(datatype_impl<T>(), N);
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
        return GenH5::DataType::varLen(datatype_impl<T>());
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

        GenH5::mpl::static_for<sizeof...(Ts)>([&](auto const idx){
            members.append({
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
            if (m_typeNames[i].isEmpty())
            {
                // String::number ambiguous for size_t
                m_typeNames[i] = String{"type_"} +
                                 String::number(static_cast<qulonglong>(i));
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

// operators
/**
 * @brief operator == Compares two datatypes. Returns true if the two types
 * are similar to each other (e.g. same size, class type, super type, members
 * etc.). Some properties are not compared entirely (e.g. precision of floating
 * point types, endianess, signedness, enum values etc).
 * @param first First
 * @param other Other
 * @return are types mostly equal
 */
GENH5_EXPORT bool operator==(GenH5::DataType const& first,
                             GenH5::DataType const& other);
GENH5_EXPORT bool operator!=(GenH5::DataType const& first,
                             GenH5::DataType const& other);

/**
 * @brief operator == Compares two compound members. Returns true if the two
 * types are exactly the same (e.g. same type, same offset, same name).
 * @param first First
 * @param other Other
 * @return equal
 */
GENH5_EXPORT bool operator==(GenH5::CompoundMember const& first,
                             GenH5::CompoundMember const& other);
GENH5_EXPORT bool operator!=(GenH5::CompoundMember const& first,
                             GenH5::CompoundMember const& other);

inline void
swap(GenH5::DataType& a, GenH5::DataType& b) noexcept
{
    a.swap(b);
}

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

// default datatypes
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

GENH5_DECLARE_DATATYPE(GenH5::Version, DataType::Version);

// fixed string
template <size_t N>
struct GenH5::details::datatype_impl<GenH5::Array<char, N>> {
    datatype_impl(CompoundNames<0> = {}) {}
    operator GenH5::DataType() const { return GenH5::DataType::string(N); }
};

#endif // GENH5_DATATYPE_H
