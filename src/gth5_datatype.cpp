/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_datatype.h"
#include "gth5_utils.h"

#include <QDebug>

GtH5::DataType
GtH5::DataType::VarString{GtH5::DataType::string(H5T_VARIABLE)};

GtH5::DataType
GtH5::DataType::Char{H5::PredType::NATIVE_CHAR};

GtH5::DataType
GtH5::DataType::Int{H5::PredType::NATIVE_INT};
GtH5::DataType
GtH5::DataType::Long{H5::PredType::NATIVE_LONG};
GtH5::DataType
GtH5::DataType::LLong{H5::PredType::NATIVE_LLONG};
GtH5::DataType
GtH5::DataType::UInt{H5::PredType::NATIVE_UINT};
GtH5::DataType
GtH5::DataType::ULong{H5::PredType::NATIVE_ULONG};
GtH5::DataType
GtH5::DataType::ULLong{H5::PredType::NATIVE_ULLONG};

GtH5::DataType
GtH5::DataType::Float{H5::PredType::NATIVE_FLOAT};
GtH5::DataType
GtH5::DataType::Double{H5::PredType::NATIVE_DOUBLE};

GtH5::DataType::DataType() = default;

GtH5::DataType
GtH5::DataType::string(size_t size, bool useUtf8) noexcept(false)
{
    auto dtype = DataType{H5::StrType{H5::PredType::C_S1, size}};
    if (useUtf8)
    {
        H5Tset_cset(dtype.id(), H5T_CSET_UTF8);
    }
    return dtype;
}

GtH5::DataType
GtH5::DataType::array(DataType const& type,
                    Dimensions const& dims) noexcept(false)
{
    return DataType{H5::ArrayType{type.toH5(), dims.length(),
                                  dims.constData()}};
}

GtH5::DataType
GtH5::DataType::array(DataType const& type, hsize_t length) noexcept(false)
{
    return array(type, Dimensions{length});
}

GtH5::DataType
GtH5::DataType::varLen(DataType const& type) noexcept(false)
{
    return DataType{H5::VarLenType{type.toH5()}};
}

GtH5::DataType
GtH5::DataType::compound(size_t dataSize,
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

GtH5::DataType::DataType(H5::DataType type) :
    m_datatype{std::move(type)}
{ }

hid_t
GtH5::DataType::id() const noexcept
{
    return m_datatype.getId();
}

bool
GtH5::DataType::isArray() const noexcept
{
    return type() == Type::H5T_ARRAY;
}

bool
GtH5::DataType::isCompound() const noexcept
{
    return type() == Type::H5T_COMPOUND;
}

bool
GtH5::DataType::isVarLen() const noexcept
{
    return type() == Type::H5T_VLEN;
}

bool
GtH5::DataType::isVarString() const noexcept
{
    return H5Tis_variable_str(id());
}

H5::DataType const&
GtH5::DataType::toH5() const  noexcept
{
    return m_datatype;
}

size_t
GtH5::DataType::size() const noexcept
{
    return H5Tget_size(id());
}

GtH5::DataType::Type
GtH5::DataType::type() const noexcept
{
    return isValid() ? H5Tget_class(id()) : H5T_NO_CLASS;
}

GtH5::Dimensions
GtH5::DataType::arrayDimensions() const  noexcept
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

GtH5::CompoundMembers
GtH5::DataType::compoundMembers() const
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

GtH5::DataType
GtH5::DataType::superType() const noexcept(false)
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
GtH5::String
GtH5::DataType::toString() const
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
operator==(GtH5::DataType const& first, GtH5::DataType const& other)
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
//        qCritical() << "HDF5: [EXCEPTION] GtH5::DataType:operator== failed!";
//        return false;
//    }
}

bool
operator!=(GtH5::DataType const& first, GtH5::DataType const& other)
{
    return !(first == other);
}

bool
operator==(GtH5::CompoundMember const& first, GtH5::CompoundMember const& other)
{
    return first.name == other.name &&
           first.type == other.type &&
           first.offset == other.offset;
}
bool
operator!=(GtH5::CompoundMember const& first, GtH5::CompoundMember const& other)
{
    return !(first == other);
}
