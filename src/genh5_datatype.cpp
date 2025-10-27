/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_datatype.h"
#include "genh5_utils.h"
#include "genh5_private.h"

#include <H5Tpublic.h>

namespace GenH5
{

/// Helper function for creating a datatype
template <typename Functor, typename CharType>
inline DataType makeType(Functor&& functor, CharType&& msg)
{
    return details::make<DataType, DataTypeException>(
        std::forward<Functor>(functor), std::forward<CharType>(msg)
    );
}

/// Helper function for creating a predefined datatype
inline DataType makePredType(hid_t id)
{
    return GenH5::DataType::fromId(H5Tcopy(id));
}

} // namespace GenH5

GenH5::DataType const&
GenH5::DataType::Bool()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_HBOOL);
    return type;
}
GenH5::DataType const& GenH5::DataType::Char()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_CHAR);
    return type;
}

GenH5::DataType const& GenH5::DataType::Int8()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_INT8);
    return type;
}
GenH5::DataType const& GenH5::DataType::Int16()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_INT16);
    return type;
}
GenH5::DataType const& GenH5::DataType::Int32()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_INT32);
    return type;
}
GenH5::DataType const& GenH5::DataType::Int64()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_INT64);
    return type;
}

GenH5::DataType const& GenH5::DataType::UInt8()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_UINT8);
    return type;
}
GenH5::DataType const& GenH5::DataType::UInt16()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_UINT16);
    return type;
}
GenH5::DataType const& GenH5::DataType::UInt32()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_UINT32);
    return type;
}
GenH5::DataType const& GenH5::DataType::UInt64()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_UINT64);
    return type;
}

GenH5::DataType const& GenH5::DataType::SChar()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_SCHAR);
    return type;
}
GenH5::DataType const& GenH5::DataType::Short()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_SHORT);
    return type;
}
GenH5::DataType const& GenH5::DataType::Int()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_INT);
    return type;
}
GenH5::DataType const& GenH5::DataType::Long()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_LONG);
    return type;
}
GenH5::DataType const& GenH5::DataType::LLong()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_LLONG);
    return type;
}

GenH5::DataType const& GenH5::DataType::UChar()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_UCHAR);
    return type;
}
GenH5::DataType const& GenH5::DataType::UShort()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_USHORT);
    return type;
}
GenH5::DataType const& GenH5::DataType::UInt()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_UINT);
    return type;
}
GenH5::DataType const& GenH5::DataType::ULong()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_ULONG);
    return type;
}
GenH5::DataType const& GenH5::DataType::ULLong()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_ULLONG);
    return type;
}

GenH5::DataType const& GenH5::DataType::Float()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_FLOAT);
    return type;
}
GenH5::DataType const& GenH5::DataType::Double()
{
    static DataType type = GenH5::makePredType(H5T_NATIVE_DOUBLE);
    return type;
}

GenH5::DataType const& GenH5::DataType::VarString()
{
    static DataType type = GenH5::DataType::varString();
    return type;
}

GenH5::DataType const& GenH5::DataType::Version()
{
    static DataType type = GenH5::DataType::compound(sizeof(GenH5::Version), {
        {"major", offsetof(GenH5::Version, major), GenH5::dataType<int>()},
        {"minor", offsetof(GenH5::Version, minor), GenH5::dataType<int>()},
        {"patch", offsetof(GenH5::Version, patch), GenH5::dataType<int>()}
    });
    return type;
}

GenH5::DataType
GenH5::DataType::string(size_t size, bool useUtf8) noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to create string type";

    DataType dtype = makeType([=](){
        return H5Tcreate(H5T_STRING, size);
    }, errMsg);

    if (useUtf8 && H5Tset_cset(dtype.m_id, H5T_CSET_UTF8) < 0)
    {
        throw DataTypeException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to enable utf8 for string type"
        };
    }

    return dtype;
}

GenH5::DataType
GenH5::DataType::varString(bool useUtf8) noexcept(false)
{
    return string(H5T_VARIABLE, useUtf8);
}

GenH5::DataType
GenH5::DataType::array(DataType const& type,
                       Dimensions const& dims) noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to create array type";

    return makeType(
                [id = type.m_id, len = dims.length(), ptr = dims.constData()](){
        return H5Tarray_create(id, len, ptr);
    }, errMsg);
}

GenH5::DataType
GenH5::DataType::array(DataType const& type, hsize_t length) noexcept(false)
{
    return array(type, Dimensions{length});
}

GenH5::DataType
GenH5::DataType::varLen(DataType const& type) noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to create varlen type";

    return makeType([id = type.m_id](){
        return H5Tvlen_create(id);
    }, errMsg);
}

GenH5::DataType
GenH5::DataType::compound(size_t dataSize,
                          CompoundMembers const& members) noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to create compound type";

    if (dataSize < 1)
    {
        throw DataTypeException{
            GENH5_MAKE_EXECEPTION_STR()
            "Failed to create compound type (data size must be positive)"
        };
    }

    // check if types are valid
    int i = 0;
    for (auto const& m : qAsConst(members))
    {
        if (m.name.isEmpty())
        {
            throw DataTypeException{
                GENH5_MAKE_EXECEPTION_STR()
                "Failed to create compound type (member no. " +
                std::to_string(i) + ": invalid name)"
            };
        }
        if (!m.type.isValid())
        {
            throw DataTypeException{
                GENH5_MAKE_EXECEPTION_STR()
                "Failed to create compound type (member no. " +
                std::to_string(i) + ": invalid type)"
            };
        }
        ++i;
    }

    // create type
    DataType dtype = makeType([=](){
        return H5Tcreate(H5T_COMPOUND, dataSize);
    }, errMsg);

    // insert members
    for (i = 0; i < members.length(); ++i)
    {
        auto const& m = members.at(i);

        if (H5Tinsert(dtype.m_id, m.name.constData(), m.offset, m.type.m_id) < 0)
        {
            throw DataTypeException{
                GENH5_MAKE_EXECEPTION_STR() "Failed to insert member no. " +
                std::to_string(i) + " into compound type (offset: " +
                std::to_string(m.offset) + ", size: " +
                std::to_string(m.type.size()) + " vs. datasize: " +
                std::to_string(dataSize) + ")"
            };
        }
    }

    return dtype;
}

GenH5::DataType
GenH5::DataType::fromId(hid_t id) noexcept
{
    DataType d;
    d.m_id = id;
    return d;
}

GenH5::DataType::DataType() = default;

GenH5::DataType::DataType(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

hid_t
GenH5::DataType::id() const noexcept
{
    return m_id;
}

bool
GenH5::DataType::isInt() const noexcept
{
    return type() == DataTypeClass::Int;
}

bool
GenH5::DataType::isSigned() const noexcept
{
    return H5Tget_sign(id()) == H5T_SGN_2;
}

bool
GenH5::DataType::isFloat() const noexcept
{
    return type() == DataTypeClass::Float;
}

bool
GenH5::DataType::isString() const noexcept
{
    return type() == DataTypeClass::String;
}

bool
GenH5::DataType::isUtf8() const noexcept
{
    return H5Tget_cset(id()) == H5T_CSET_UTF8;
}

bool
GenH5::DataType::isArray() const noexcept
{
    return type() == DataTypeClass::Array;
}

bool
GenH5::DataType::isCompound() const noexcept
{
    return type() == DataTypeClass::Compound;
}

bool
GenH5::DataType::isVarLen() const noexcept
{
    return type() == DataTypeClass::VarLen;
}

bool
GenH5::DataType::isVarString() const noexcept
{
    return isString() && H5Tis_variable_str(id());
}

size_t
GenH5::DataType::size() const noexcept
{
    return H5Tget_size(m_id);
}

GenH5::DataTypeClass
GenH5::DataType::type() const noexcept
{
    return isValid() ? static_cast<DataTypeClass>(H5Tget_class(m_id)) : DataTypeClass::Error;
}

GenH5::Dimensions
GenH5::DataType::arrayDimensions() const noexcept
{
    if (!isArray())
    {
        return {};
    }

    Dimensions dims;
    dims.resize(H5Tget_array_ndims(m_id));
    H5Tget_array_dims(m_id, dims.data());
    return dims;
}

GenH5::CompoundMembers
GenH5::DataType::compoundMembers() const
{
    if (!isCompound())
    {
        return {};
    }

    int n = H5Tget_nmembers(m_id);

    if (n < 1)
    {
        return {};
    }

    CompoundMembers members;
    members.reserve(n);

    for (uint i = 0; i < static_cast<uint>(n); ++i)
    {
        char* str = H5Tget_member_name(m_id, i);
        String memberName{str};
        H5free_memory(str);

        size_t offset = H5Tget_member_offset(m_id, i);

        hid_t type = H5Tget_member_type(m_id, i);
        if (type <= 0)
        {
            throw DataTypeException{
                GENH5_MAKE_EXECEPTION_STR()
                "Failed to access datatype of compound member no. " +
                std::to_string(i)
            };
        }

        members.append({
            std::move(memberName),
            offset,
            DataType::fromId(type)
        });
    }

    return members;
}

GenH5::DataType
GenH5::DataType::superType() const noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to open super type";

    if (!isValid())
    {
        throw DataTypeException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to open super type (invalid id)"
        };
    }

    return makeType([id = m_id](){
        return H5Tget_super(id);
    }, errMsg);
}

void
GenH5::DataType::swap(DataType& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
}

#if 0
GenH5::String
GenH5::DataType::toString() const
{
    String string;

    switch (type())
    {
    case H5T_INTEGER:
        string += "Int(";
        string += String::number(size());
        string += ")";
        break;
    case H5T_FLOAT:
        string += "Float(";
        string += String::number(size());
        string += ")";
        break;
    case H5T_STRING:
        if (isVarString()) string += "Var";
        string += "String(";
        string += String::number(size());
        string += ")";
        break;
    case H5T_VLEN:
        string += "VarLen{ "
               +  superType().toString()
               +  " }";
        break;
    case H5T_COMPOUND:
        {
            string += "CompMember[";
            bool changed = false;
            for (auto const& m : compoundMembers())
            {
                changed = true;
                string += "{ \""
                       +  m.name + "\", "
                       +  m.type.toString() + ", at "
                       +  String::number(static_cast<qulonglong>(m.offset))
                       +  " }, ";
            }
            if (changed) string.remove(string.size() - 2, 2);
            string += "]";
        }
        break;
    case H5T_ARRAY:
        string += "Array{ ";
        {
            QString s;
            QDebug d(&s);
            d.nospace() << arrayDimensions();
            string += s.toUtf8();
        }
        string += " x ";
        string += superType().toString();
        string += " }";
        break;
    case H5T_OPAQUE:
         string += "Opaque";
        break;
    case H5T_BITFIELD:
         string += "Bitfield";
        break;
    case H5T_ENUM:
         string += "Enum";
        break;
    case H5T_TIME:
         string += "Time";
        break;
    case H5T_REFERENCE:
         string += "Ref";
        break;
    default:
         string += "Type{ "
                + String::number(type())
                + " " + String::number(id())
                + " }";
    }

    return string;
}
#endif

bool
operator==(GenH5::DataType const& first, GenH5::DataType const& other)
{
    // same id -> nothing to do here
    if (first.id() == other.id())
    {
        return true;
    }

    GenH5::DataTypeClass const classType = first.type();

    // class types differ -> exit
    if (classType != other.type())
    {
        return false;
    }

    bool const isSameSize = first.size() == other.size();

    // some indepth checking
    switch (classType)
    {
    case GenH5::DataTypeClass::Enum:
        // TODO: Check enum values and names
        return isSameSize &&
               H5Tget_nmembers(first.id()) == H5Tget_nmembers(other.id());
    case GenH5::DataTypeClass::Int: // not checking endianess etc.
    case GenH5::DataTypeClass::Float: // not checking precision etc.
        return isSameSize;
    case GenH5::DataTypeClass::String:
        return isSameSize &&
               first.isVarString() == other.isVarString();
    case GenH5::DataTypeClass::VarLen:
        if (isSameSize)
        {
            auto const fSuper = first.superType();
            auto const oSuper = other.superType();
            return fSuper == oSuper;
        }
        break;
    case GenH5::DataTypeClass::Array:
        if (isSameSize)
        {
            auto const fDims = first.arrayDimensions();
            auto const oDims = other.arrayDimensions();
            auto const fSuper = first.superType();
            auto const oSuper = other.superType();
            return fDims == oDims && fSuper == oSuper;
        }
        break;
    case GenH5::DataTypeClass::Compound:
        if (isSameSize)
        {
            auto const fMembers = first.compoundMembers();
            auto const oMembers = other.compoundMembers();
            int size = fMembers.size();
            if (size == oMembers.size())
            {
                for (int i = 0; i < size; ++i)
                {
                    if (fMembers[i] != oMembers[i])
                    {
                        return false;
                    }
                }
                return true;
            }
        }
        break;
    case GenH5::DataTypeClass::Opaque:
        if (isSameSize)
        {
            char* fTag = H5Tget_tag(first.id());
            char* oTag = H5Tget_tag(other.id());
            bool const isSameTag = fTag == oTag;
            free(fTag);
            free(oTag);
            return isSameSize && isSameTag;
        }
        break;
    case GenH5::DataTypeClass::BitField:
    case GenH5::DataTypeClass::Time:
    case GenH5::DataTypeClass::Reference:
    default:
        return isSameSize;
    }

    return false;
}

bool
operator!=(GenH5::DataType const& first, GenH5::DataType const& other)
{
    return !(first == other);
}

bool
operator==(GenH5::CompoundMember const& first,
           GenH5::CompoundMember const& other)
{
    return first.offset == other.offset &&
           first.name == other.name &&
           first.type == other.type;
}
bool
operator!=(GenH5::CompoundMember const& first,
           GenH5::CompoundMember const& other)
{
    return !(first == other);
}
