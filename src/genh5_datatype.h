/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATATYPE_H
#define GENH5_DATATYPE_H

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

    static DataType Bool;
    static DataType Char;

    static DataType Int;
    static DataType Long;
    static DataType LLong;
    static DataType UInt;
    static DataType ULong;
    static DataType ULLong;

    static DataType Float;
    static DataType Double;

    static DataType Version;
    static DataType VarString;

    /// will create an string type with utf8 enabled by default
    static DataType string(size_t size, bool useUtf8 = true) noexcept(false);

    static DataType array(DataType const& type,
                          Dimensions const& dims) noexcept(false);
    static DataType array(DataType const& type, hsize_t length) noexcept(false);

    static DataType varLen(DataType const& type) noexcept(false);

    static DataType compound(size_t dataSize,
                             CompoundMembers const& members) noexcept(false);

    DataType();
    explicit DataType(H5::DataType type);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataType const& toH5() const noexcept;

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

protected:

    /// hdf5 base instance
    H5::DataType m_datatype{};
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
    auto members = dtype.compoundMembers();

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
        return GenH5::DataType::array(datatype_impl<T>(), N);
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

        GenH5::mpl::static_rfor<sizeof...(Ts)>(
                    [&](auto const idx, auto const ridx){
            members.append({
                m_typeNames.at(ridx),
                offset<idx>(),
                datatype_impl<std::tuple_element_t<ridx, Comp<Ts...>>>()
            });
        });
        // members are flipped, not necessary to reverse, but makes
        // debugging easier
        std::reverse(std::begin(members), std::end(members));

        return DataType::compound(sizeof(Compound), members);
    }

private:
    /// compound member for calculating offset of an element
    Compound m_t;
    /// names for compound members
    TypeNames m_typeNames{};

    /// offset of tuple element
    template<size_t Idx>
    size_t offset() const
    {
        return reinterpret_cast<size_t>(&get<Idx>(m_t)) -
               reinterpret_cast<size_t>(&m_t);
    }

    /// generate compound member names if missing
    void generateDefaultNames()
    {
        for (size_t i = 0; i < m_typeNames.size(); ++i)
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

} // namespace details

// compound types
template<typename T1, typename T2, typename... Tother>
inline DataType
dataType(CompoundNames<sizeof...(Tother)+2> memberNames = {}) noexcept(false)
{
    using T = Comp<T1, T2, Tother...>;
    return details::datatype_impl<T>(std::move(memberNames));
}

// compound names
template<typename T>
inline DataType
dataType(CompoundNames<traits::comp_size<T>::value> memberNames) noexcept(false)
{
    return details::datatype_impl<T>(std::move(memberNames));
}

template<typename T>
inline DataType
dataType() noexcept(false)
{
    return details::datatype_impl<T>();
}

} // namespace GenH5

// operators
GENH5_EXPORT bool operator==(GenH5::DataType const& first,
                            GenH5::DataType const& other);
GENH5_EXPORT bool operator!=(GenH5::DataType const& first,
                            GenH5::DataType const& other);

GENH5_EXPORT bool operator==(GenH5::CompoundMember const& first,
                            GenH5::CompoundMember const& other);
GENH5_EXPORT bool operator!=(GenH5::CompoundMember const& first,
                            GenH5::CompoundMember const& other);

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
struct GenH5::details::datatype_impl<char[N]> {
    datatype_impl(CompoundNames<0> = {}) {}
    operator GenH5::DataType() const { return GenH5::DataType::string(N); }
};

#endif // GENH5_DATATYPE_H
