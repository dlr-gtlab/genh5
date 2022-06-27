/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_datatype.h"
#include "genh5_utils.h"

#include <QDebug>

GenH5::DataType
GenH5::DataType::Bool{H5::PredType::NATIVE_HBOOL};
GenH5::DataType
GenH5::DataType::Char{H5::PredType::NATIVE_CHAR};

GenH5::DataType
GenH5::DataType::Int{H5::PredType::NATIVE_INT};
GenH5::DataType
GenH5::DataType::Long{H5::PredType::NATIVE_LONG};
GenH5::DataType
GenH5::DataType::LLong{H5::PredType::NATIVE_LLONG};
GenH5::DataType
GenH5::DataType::UInt{H5::PredType::NATIVE_UINT};
GenH5::DataType
GenH5::DataType::ULong{H5::PredType::NATIVE_ULONG};
GenH5::DataType
GenH5::DataType::ULLong{H5::PredType::NATIVE_ULLONG};

GenH5::DataType
GenH5::DataType::Float{H5::PredType::NATIVE_FLOAT};
GenH5::DataType
GenH5::DataType::Double{H5::PredType::NATIVE_DOUBLE};

GenH5::DataType
GenH5::DataType::VarString{GenH5::DataType::string(H5T_VARIABLE)};

GenH5::DataType
GenH5::DataType::Version{GenH5::DataType::compound(sizeof(GenH5::Version), {
    {"major", offsetof(GenH5::Version, major), dataType<int>()},
    {"minor", offsetof(GenH5::Version, minor), dataType<int>()},
    {"patch", offsetof(GenH5::Version, patch), dataType<int>()}
})};

GenH5::DataType::DataType() = default;

GenH5::DataType
GenH5::DataType::string(size_t size, bool useUtf8) noexcept(false)
{
    auto dtype = DataType{H5::StrType{H5::PredType::C_S1, size}};
    if (useUtf8)
    {
        H5Tset_cset(dtype.id(), H5T_CSET_UTF8);
    }
    return dtype;
}

GenH5::DataType
GenH5::DataType::array(DataType const& type,
                    Dimensions const& dims) noexcept(false)
{
    return DataType{H5::ArrayType{type.toH5(), dims.length(),
                                  dims.constData()}};
}

GenH5::DataType
GenH5::DataType::array(DataType const& type, hsize_t length) noexcept(false)
{
    return array(type, Dimensions{length});
}

GenH5::DataType
GenH5::DataType::varLen(DataType const& type) noexcept(false)
{
    return DataType{H5::VarLenType{type.toH5()}};
}

GenH5::DataType
GenH5::DataType::compound(size_t dataSize,
                         CompoundMembers const& members) noexcept(false)
{
    if (dataSize < 1)
    {
        throw DataTypeException{"Creating compound type failed "
                                "(Data size must be positive)"};
    }

    for (auto const& m : members)
    {
        if (m.name.isEmpty())
        {
            throw DataTypeException{"Creating compound type failed "
                                    "(Invalid member name)"};
        }
        if (!m.type.isValid())
        {
            throw DataTypeException{"Creating compound type failed "
                                    "(Invalid member type)"};
        }
    }

    H5::CompType dtype{dataSize};

    for (int i = 0; i < members.length(); ++i)
    {
        auto const& m = members.at(i);

        if (H5Tinsert(dtype.getId(), m.name.constData(), m.offset, m.type.id()))
        {
            qCritical().nospace().noquote()
                    << "HDF5: Failed to insert member no. " << i << " '"
                    << m.name << "' into compound type! (offset: " << m.offset
                    << ", size: " << m.type.size()
                    << " vs. datasize: " << dataSize << ")";
            throw DataTypeException{"Failed to insert member into "
                                    "compound type"};
        }
    }

    return DataType{dtype};
}

GenH5::DataType::DataType(H5::DataType type) :
    m_datatype{std::move(type)}
{ }

hid_t
GenH5::DataType::id() const noexcept
{
    return m_datatype.getId();
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
    return H5Tis_variable_str(id());
}

H5::DataType const&
GenH5::DataType::toH5() const  noexcept
{
    return m_datatype;
}

size_t
GenH5::DataType::size() const noexcept
{
    return H5Tget_size(id());
}

GenH5::DataType::Type
GenH5::DataType::type() const noexcept
{
    return isValid() ? H5Tget_class(id()) : H5T_NO_CLASS;
}

GenH5::Dimensions
GenH5::DataType::arrayDimensions() const  noexcept
{
    if (!isArray())
    {
        return {};
    }

    Dimensions dims;
    dims.resize(H5Tget_array_ndims(id()));
    H5Tget_array_dims(id(), dims.data());
    return dims;
}

GenH5::CompoundMembers
GenH5::DataType::compoundMembers() const
{
    if (!isCompound())
    {
        return {};
    }

    H5::CompType dtype(id());

    int n = dtype.getNmembers();

    if (n < 1)
    {
        return {};
    }

    CompoundMembers members;
    members.reserve(n);

    try
    {
        for (uint i = 0; i < static_cast<uint>(n); ++i)
        {
            auto* str = H5Tget_member_name(dtype.getId(), i);
            String memberName{str};
            H5free_memory(str);
            members.append({
               memberName,
               dtype.getMemberOffset(i),
               DataType{dtype.getMemberDataType(i)}
           });
        }
    }
    catch (H5::DataTypeIException const& e)
    {
        throw DataTypeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataType::compoundMembers";
        throw DataTypeException{e.getCDetailMsg()};
    }

    return members;
}

GenH5::DataType
GenH5::DataType::superType() const noexcept(false)
{
    try
    {
        return DataType{m_datatype.getSuper()};
    }
    catch (H5::DataTypeIException const& e)
    {
        throw DataTypeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataType::compoundMembers";
        throw DataTypeException{e.getCDetailMsg()};
    }
}

#if 0
GenH5::String
GenH5::DataType::toString() const
{
    String string;

    switch (type())
    {
    case H5T_INTEGER:
        string += "Int";
        break;
    case H5T_FLOAT:
        string += "Float";
        break;
    case H5T_STRING:
        string += "String";
        break;
    case H5T_VLEN:
        string += "VarLen{ "
               +  superType().toString()
               +  " }";
        break;
    case H5T_COMPOUND:
        {
            for (auto const& m : compoundMembers())
            {
                string += "CompMember{ \""
                       +  m.name + "\", "
                       +  m.type.toString() + ", at "
                       +  String::number(m.offset)
                       +  " }, ";
            }
            string.remove(string.size()-1-2, 2);
        }
        break;
    case H5T_ARRAY:
        string += "Array{ "
               +  arrayDimensions()
               +  " x "
               +  superType()
               +  " }";
        break;
    case H5T_OPAQUE:
        stream << "Opaque";
        break;
    case H5T_BITFIELD:
        stream << "Bitfield";
        break;
    case H5T_ENUM:
        stream << "Enum";
        break;
    case H5T_TIME:
        stream << "Time";
        break;
    case H5T_REFERENCE:
        stream << "Ref";
        break;
    default:
        stream << "Type{ "
               << dtype.type()
               << " }";
    }

    return stream;
}
#endif

bool
operator==(GenH5::DataType const& first, GenH5::DataType const& other)
{
    return first.id() == other.id() || (
               first.type() == other.type() &&
               first.size() == other.size());

//    try
//    {
//        // check isValid to elimante "not a datatype" error from hdf5
//        return first.id() == other.id() || (first.isValid() &&
//                                            other.isValid() &&
//                                            first.toH5() == other.toH5());
//    }
//    catch (H5::DataTypeIException const& /*e*/)
//    {
//        qWarning() << "HDF5: Datatype comparisson failed! (invalid data type)";
//        return false;
//    }
//    catch (H5::Exception const& /*e*/)
//    {
//        qCritical() << "HDF5: [EXCEPTION] GenH5::DataType:operator== failed!";
//        return false;
//    }
}

bool
operator!=(GenH5::DataType const& first, GenH5::DataType const& other)
{
    return !(first == other);
}

bool
operator==(GenH5::CompoundMember const& first, GenH5::CompoundMember const& other)
{
    return first.name == other.name &&
           first.type == other.type &&
           first.offset == other.offset;
}
bool
operator!=(GenH5::CompoundMember const& first, GenH5::CompoundMember const& other)
{
    return !(first == other);
}
