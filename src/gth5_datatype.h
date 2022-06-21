/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATATYPE_H
#define GTH5_DATATYPE_H

#include "gth5_object.h"
#include "gth5_conversion/type.h"
#include "gth5_typetraits.h"
#include <QDebug>

namespace GtH5
{
// forward decl
struct CompoundMember;

using CompoundMembers = Vector<CompoundMember>;

/**
 * @brief The DataType class
 */
class GTH5_EXPORT DataType : public Object
{
public:

    using Type = H5T_class_t;

    static DataType VarString;

    static DataType Char;

    static DataType Int;
    static DataType Long;
    static DataType LLong;
    static DataType UInt;
    static DataType ULong;
    static DataType ULLong;

    static DataType Float;
    static DataType Double;

    /// will create an string type with utf8 enabled by default
    static DataType string(size_t size, bool useUtf8 = true);

    static DataType array(DataType const& type,
                          Dimensions const& dims);
    static DataType array(DataType const& type, hsize_t length);

    static DataType varLen(DataType const& type);

    static DataType compound(size_t dataSize,
                             CompoundMembers const& members);

    DataType();
    explicit DataType(H5::DataType type);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataType const& toH5() const;

    /**
     * @brief size
     * @return size of a datatype in bytes.
     */
    size_t size() const;

    /**
     * @brief type of this datatype (compound, array etc)
     * @return type
     */
    Type type() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const override;

    /**
     * @brief whether this datatype is a fixed sized array.
     * @return true if is array type
     */
    bool isArray() const;

    /**
     * @brief whether this datatype is a fixed sized array.
     * @return true if is compound type
     */
    bool isCompound() const;

    /**
     * @brief  whether this datatype is a variable length datatype.
     * Will return false if type is var string.
     * @return true if is var len.
     */
    bool isVarLen() const;

    /**
     * @brief  whether this datatype is a variable length string.
     * @return true if is var string
     */
    bool isVarString() const;

    /**
     * @brief the dimensions of the fixed sized array datatype.
     * @return array dimensions. empty if not an array or process failed
     */
    Dimensions arrayDimensions() const;

    /**
     * @brief Returns the compound members of the compound datatype.
     * @return members. Empty if not an compound type
     */
    CompoundMembers compoundMembers() const;

    /**
     * @brief Returns the super datatype of this type. Used for accessing the
     * underlying type of a var len type.
     * @return super datatype
     */
    DataType superType() const;

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
getTypeNames(DataType const& dtype)
{
    auto members = dtype.compoundMembers();

    CompoundNames<N> names{};

    ssize_t offset = members.size()-N;
    if (offset >= 0)
    {
        std::transform(std::begin(members), std::end(members)-offset,
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
struct datatype_impl;

// specialization for varlen types
template <typename T>
struct datatype_impl<VarLen<T>>
{
    datatype_impl(CompoundNames<0> = {}) {}
    operator DataType() const
    {
        return GtH5::DataType::varLen(datatype_impl<T>());
    }
};

// specialization for array types
template <typename T, size_t N>
struct datatype_impl<Array<T, N>>
{
    datatype_impl(CompoundNames<0> = {}) {}
    operator DataType() const
    {
        return GtH5::DataType::array(datatype_impl<T>(), N);
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

        GtH5::mpl::static_rfor<sizeof...(Ts)>([&](auto const idx,
                                             auto const ridx){
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
        return reinterpret_cast<size_t>(&std::get<Idx>(m_t)) -
               reinterpret_cast<size_t>(&m_t);
    }

    /// generate compound member names if missing
    void generateDefaultNames()
    {
        for (int i = 0; i < m_typeNames.size(); ++i)
        {
            if (m_typeNames[i].isEmpty())
            {
                m_typeNames[i] = String{"type_"} + String::number(i);
            }
        }
    }
};

} // namespace details

// compound type
template<typename T1, typename T2, typename... Tother>
inline DataType
dataType(CompoundNames<sizeof...(Tother)+2> memberNames = {})
{
    using T = Comp<T1, T2, Tother...>;
    return details::datatype_impl<T>(std::move(memberNames));
}

// T == compound<...>
template<typename T>
inline DataType
dataType(CompoundNames<traits::comp_size<T>::value> memberNames)
{
    // T must have an datatype associated!
    return details::datatype_impl<T>(std::move(memberNames));
}

template<typename T>
inline DataType
dataType()
{
    // T must have an datatype associated!
    return details::datatype_impl<T>();
}

} // namespace GtH5

// operators
GTH5_EXPORT bool operator==(GtH5::DataType const& first,
                            GtH5::DataType const& other);
GTH5_EXPORT bool operator!=(GtH5::DataType const& first,
                            GtH5::DataType const& other);

GTH5_EXPORT bool operator==(GtH5::CompoundMember const& first,
                            GtH5::CompoundMember const& other);
GTH5_EXPORT bool operator!=(GtH5::CompoundMember const& first,
                            GtH5::CompoundMember const& other);

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5DataType = GtH5::DataType;
#endif

GTH5_EXPORT std::ostream& operator<<(std::ostream& s, GtH5::DataType const& d);
GTH5_EXPORT std::ostream& operator<<(std::ostream& s,
                                     GtH5::CompoundMember const& d);

GTH5_EXPORT QDebug operator<<(QDebug s, GtH5::DataType const& d);
GTH5_EXPORT QDebug operator<<(QDebug s, GtH5::CompoundMember const& d);

#define GTH5_DECLARE_DATATYPE(NATIVE_TYPE, H5_TYPE) \
    template <> \
    struct GtH5::details::datatype_impl<NATIVE_TYPE> { \
        datatype_impl(CompoundNames<0> = {}) {} \
        operator GtH5::DataType() const { return H5_TYPE; } \
    };

#define GTH5_DECLARE_DATATYPE_IMPL(NATIVE_TYPE) \
    template <> \
    struct GtH5::details::datatype_impl<NATIVE_TYPE> { \
        datatype_impl(CompoundNames<0> = {}) {} \
        operator GtH5::DataType() const; \
    }; \
    inline \
    GtH5::details::datatype_impl<NATIVE_TYPE>::operator GtH5::DataType() const

// default datatypes
GTH5_DECLARE_DATATYPE(char, DataType::Char);

GTH5_DECLARE_DATATYPE(char*, DataType::VarString);
GTH5_DECLARE_DATATYPE(char const*, DataType::VarString);

GTH5_DECLARE_DATATYPE(int, DataType::Int);
GTH5_DECLARE_DATATYPE(long int, DataType::Long);
GTH5_DECLARE_DATATYPE(long long int, DataType::LLong);
GTH5_DECLARE_DATATYPE(unsigned int, DataType::UInt);
GTH5_DECLARE_DATATYPE(unsigned long int, DataType::ULong);
GTH5_DECLARE_DATATYPE(unsigned long long int, DataType::ULLong);

GTH5_DECLARE_DATATYPE(float, DataType::Float);
GTH5_DECLARE_DATATYPE(double, DataType::Double);

#endif // GTH5_DATATYPE_H
