/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_abtsractdataset.h"


GtH5::AbstractDataSet::AbstractDataSet() = default;

bool
GtH5::AbstractDataSet::write(void const* data, Optional<DataType> dtype) const
{
    auto space = dataSpace();
    if (space.isNull())
    {
        qCritical() << "HDF5: Writing data vector failed!"
                    << "(Null dataspace)";
        return false;
    }
    if (!data)
    {
        qCritical() << "HDF5: Writing data vector failed!"
                    << "(Data vector is invalid)";
        return false;
    }

    auto type = dataType();
    if (dtype.isDefault() || dtype == type)
    {
        dtype = type;
    }

    return doWrite(data, dtype);
}

bool
GtH5::AbstractDataSet::read(void* data, Optional<DataType> dtype) const
{

    auto space = dataSpace();
    if (space.isNull())
    {
        qCritical() << "HDF5: Reading data vector failed!"
                    << "(Null dataspace)";
        return false;
    }
    if (!data)
    {
        qCritical() << "HDF5: Reading data vector failed!"
                    << "(Data vector is invalid)";
        return false;
    }

    auto type = dataType();
    if (dtype.isDefault() || dtype == type)
    {
        dtype = type;
    }

    return doRead(data, dtype);
}

void
GtH5::AbstractDataSet::debugWriteError(size_t length) const
{
    auto dspace = dataSpace();
    qCritical() << "HDF5: Writing data failed!" <<
                   "(Too few data elements:"
                << length << "vs."
                << dspace.dimensions()
                << dspace.selectionSize() << "elements)";
}

GtH5::DataType
GtH5::AbstractDataSet::dataType() const noexcept(false)
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

GtH5::DataSpace
GtH5::AbstractDataSet::dataSpace() const noexcept(false)
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
