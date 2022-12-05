/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_abstractdataset.h"

#include <QDebug>

GenH5::AbstractDataSet::AbstractDataSet() = default;

namespace GenH5
{

/// We want to check if the other datatype is a compound with compatible
/// memory layout but has default member names. In this case we should use the
/// datatype of the dataset so that the correct member names are used
inline bool isDefaultCompoundType(GenH5::DataType dtype, GenH5::DataType other)
{
    // We have to check this only for compound types
    if (!dtype.isCompound() || !other.isCompound())
    {
        return false;
    }

    CompoundMembers const tMembers = dtype.compoundMembers();
    CompoundMembers const oMembers = other.compoundMembers();
    int size = tMembers.size();

    // Compound size must match
    if (size != oMembers.size())
    {
        return false;
    }

    for (int i = 0; i < size; ++i)
    {
        if (// the offset must be the same (identical memory layout)
            tMembers[i].offset != oMembers[i].offset ||
            // the meber type must be the same
            tMembers[i].type   != oMembers[i].type ||
            // names should not match
            tMembers[i].name   == oMembers[i].name ||
            // we expect a default compound type name
            oMembers[i].name != String{"type_"} + QString::number(i))
        {
            return false;
        }
    }

    return true;
}

} // namespace

bool
GenH5::AbstractDataSet::write(void const* data, Optional<DataType> dtype) const
{
    if (!data)
    {
        qCritical() << "HDF5: Writing data vector failed!"
                    << "(Data vector is invalid)";
        return false;
    }

    auto space = dataSpace();
    if (space.isNull())
    {
        qCritical() << "HDF5: Writing data vector failed!"
                    << "(Null dataspace)";
        return false;
    }

    auto type = dataType();
    // use the datatype of the dataset if not specified
    if (dtype.isDefault() || isDefaultCompoundType(type, dtype))
    {
        dtype = type;
    }

    return doWrite(data, dtype);
}

bool
GenH5::AbstractDataSet::read(void* data, Optional<DataType> dtype) const
{
    if (!data)
    {
        qCritical() << "HDF5: Reading data vector failed!"
                    << "(Data vector is invalid)";
        return false;
    }

    auto space = dataSpace();
    if (space.isNull())
    {
        qCritical() << "HDF5: Reading data vector failed!"
                    << "(Null dataspace)";
        return false;
    }

    auto type = dataType();
    // use the datatype of the dataset if not specified
    if (dtype.isDefault() || isDefaultCompoundType(type, dtype))
    {
        dtype = type;
    }

    return doRead(data, dtype);
}

void
GenH5::AbstractDataSet::debugWriteError(size_t length,
                                        Optional<DataSpace> const& space) const
{
    auto dspace = space.isDefault() ? dataSpace() : *space;
    qCritical() << "HDF5: Writing data failed!" <<
                   "(Too few data elements:"
                << length << "vs."
                << dspace.dimensions()
                << dspace.selectionSize() << "selected elements)";
}

void
GenH5::AbstractDataSet::debugReadError(size_t length,
                                       Optional<DataSpace> const& space) const
{
    auto dspace = space.isDefault() ? dataSpace() : *space;
    qCritical() << "HDF5: Reading data failed!" <<
                   "(Data container is too small:"
                << length << "vs."
                << dspace.dimensions()
                << dspace.selectionSize() << "selected elements)";
}

GenH5::DataType
GenH5::AbstractDataSet::dataType() const noexcept(false)
{
    try
    {
        return DataType{toH5AbsDataSet().getDataType()};
    }
    catch (H5::DataTypeIException const& e)
    {
        throw DataTypeException{e.getCDetailMsg()};
    }
    catch (H5::AttributeIException const& e)
    {
        throw DataTypeException{e.getCDetailMsg()};
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataTypeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] AbstractDataSet::dataType";
        throw DataTypeException{e.getCDetailMsg()};
    }
}

GenH5::DataSpace
GenH5::AbstractDataSet::dataSpace() const noexcept(false)
{
    try
    {
        return DataSpace{toH5AbsDataSet().getSpace()};
    }
    catch (H5::DataSpaceIException const& e)
    {
        throw DataSpaceException{e.getCDetailMsg()};
    }
    catch (H5::AttributeIException const& e)
    {
        throw DataSpaceException{e.getCDetailMsg()};
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSpaceException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] AbstractDataSet::dataSpace";
        throw DataSpaceException{e.getCDetailMsg()};
    }
}
