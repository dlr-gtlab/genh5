/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_abtsractdataset.h"


GtH5::AbstractDataSet::AbstractDataSet(DataType dtype,
                                       DataSpace dspace) :
    m_datatype(std::move(dtype)),
    m_dataspace(std::move(dspace))
{

}

bool
GtH5::AbstractDataSet::write(void const* data, Optional<DataType> dtype) const
{
    if (m_dataspace.isNull())
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

    if (dtype.isDefault() || dtype == m_datatype)
    {
        dtype = m_datatype;
    }

    return doWrite(data, dtype);
}

bool
GtH5::AbstractDataSet::read(void* data, Optional<DataType> dtype) const
{
    if (m_dataspace.isNull())
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

    if (dtype.isDefault() || dtype == m_datatype)
    {
        dtype = m_datatype;
    }

    return doRead(data, dtype);
}

GtH5::DataType const&
GtH5::AbstractDataSet::dataType() const
{
    return m_datatype;
}

GtH5::DataSpace const&
GtH5::AbstractDataSet::dataSpace() const
{
    return m_dataspace;
}
