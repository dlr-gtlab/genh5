/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATATYPE_H
#define GENH5_DATATYPE_H

#include "genh5_idcomponent.h"
#include "genh5_object.h"
#include "genh5_typedefs.h"

#include "H5Tpublic.h"

#include <algorithm>

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

    static DataType const& Reference;

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

#endif // GENH5_DATATYPE_H
