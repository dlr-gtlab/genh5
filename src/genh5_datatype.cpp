/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_datatype.h"
#include "genh5_utils.h"
#include "genh5_private.h"

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

static const GenH5::DataType s_bool{GenH5::makePredType(H5T_NATIVE_HBOOL)};
static const GenH5::DataType s_char{GenH5::makePredType(H5T_NATIVE_CHAR)};

static const GenH5::DataType s_int{GenH5::makePredType(H5T_NATIVE_INT)};
static const GenH5::DataType s_long{GenH5::makePredType(H5T_NATIVE_LONG)};
static const GenH5::DataType s_llong{GenH5::makePredType(H5T_NATIVE_LLONG)};
static const GenH5::DataType s_uint{GenH5::makePredType(H5T_NATIVE_UINT)};
static const GenH5::DataType s_ulong{GenH5::makePredType(H5T_NATIVE_ULONG)};
static const GenH5::DataType s_ullong{GenH5::makePredType(H5T_NATIVE_ULLONG)};

static const GenH5::DataType s_float{GenH5::makePredType(H5T_NATIVE_FLOAT)};
static const GenH5::DataType s_double{GenH5::makePredType(H5T_NATIVE_DOUBLE)};

static const GenH5::DataType s_varString{GenH5::DataType::string(H5T_VARIABLE)};

static const GenH5::DataType s_version{
    GenH5::DataType::compound(sizeof(GenH5::Version), {
        {"major", offsetof(GenH5::Version, major), GenH5::dataType<int>()},
        {"minor", offsetof(GenH5::Version, minor), GenH5::dataType<int>()},
        {"patch", offsetof(GenH5::Version, patch), GenH5::dataType<int>()}
    })
};

GenH5::DataType const& GenH5::DataType::Bool = s_bool;
GenH5::DataType const& GenH5::DataType::Char = s_char;

GenH5::DataType const& GenH5::DataType::Int = s_int;
GenH5::DataType const& GenH5::DataType::Long = s_long;
GenH5::DataType const& GenH5::DataType::LLong = s_llong;
GenH5::DataType const& GenH5::DataType::UInt = s_uint;
GenH5::DataType const& GenH5::DataType::ULong = s_ulong;
GenH5::DataType const& GenH5::DataType::ULLong = s_ullong;

GenH5::DataType const& GenH5::DataType::Float = s_float;
GenH5::DataType const& GenH5::DataType::Double = s_double;

GenH5::DataType const& GenH5::DataType::VarString = s_varString;
GenH5::DataType const& GenH5::DataType::Version = s_version;

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
GenH5::DataType::array(DataType const& type,
                       Dimensions const& dims) noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to create array type";

    return makeType([id = type.m_id, len = dims.size(), ptr = dims.data()](){
        return H5Tarray_create(id, numeric_cast<unsigned>(len), ptr);
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
    size_t i = 0;
    for (auto const& m : asConst(members))
    {
        if (m.name.empty())
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
    for (i = 0; i < members.size(); ++i)
    {
        auto const& m = members.at(i);

        if (H5Tinsert(dtype.m_id, m.name.data(), m.offset, m.type.m_id) < 0)
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

GenH5::DataType GenH5::DataType::fromId(hid_t id) noexcept
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
    return type() == Type::H5T_INTEGER;
}

bool
GenH5::DataType::isFloat() const noexcept
{
    return type() == Type::H5T_FLOAT;
}

bool
GenH5::DataType::isString() const noexcept
{
    return type() == Type::H5T_STRING;
}

bool
GenH5::DataType::isArray() const noexcept
{
    return type() == Type::H5T_ARRAY;
}

bool
GenH5::DataType::isCompound() const noexcept
{
    return type() == Type::H5T_COMPOUND;
}

bool
GenH5::DataType::isVarLen() const noexcept
{
    return type() == Type::H5T_VLEN;
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

GenH5::DataType::Type
GenH5::DataType::type() const noexcept
{
    return isValid() ? H5Tget_class(m_id) : H5T_NO_CLASS;
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

    for (unsigned i = 0; i < numeric_cast<unsigned>(n); ++i)
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

        members.push_back({
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
                       +  String::number(numeric_cast<qulonglong>(m.offset))
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

    H5T_class_t const classType = first.type();

    // class types differ -> exit
    if (classType != other.type())
    {
        return false;
    }

    bool const isSameSize = first.size() == other.size();

    // some indepth checking
    switch (classType)
    {
    case H5T_ENUM:
        // TODO: Check enum values and names
        return isSameSize &&
               H5Tget_nmembers(first.id()) == H5Tget_nmembers(other.id());
    case H5T_INTEGER: // not checking endianess etc.
    case H5T_FLOAT: // not checking precision etc.
        return isSameSize;
    case H5T_STRING:
        return isSameSize &&
               first.isVarString() == other.isVarString();
    case H5T_VLEN:
        if (isSameSize)
        {
            auto const fSuper = first.superType();
            auto const oSuper = other.superType();
            return fSuper == oSuper;
        }
        break;
    case H5T_ARRAY:
        if (isSameSize)
        {
            auto const fDims = first.arrayDimensions();
            auto const oDims = other.arrayDimensions();
            auto const fSuper = first.superType();
            auto const oSuper = other.superType();
            return fDims == oDims && fSuper == oSuper;
        }
        break;
    case H5T_COMPOUND:
        if (isSameSize)
        {
            auto const fMembers = first.compoundMembers();
            auto const oMembers = other.compoundMembers();
            size_t size = fMembers.size();
            if (size == oMembers.size())
            {
                for (size_t i = 0; i < size; ++i)
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
    case H5T_OPAQUE:
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
    case H5T_BITFIELD:
    case H5T_TIME:
    case H5T_REFERENCE:
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
