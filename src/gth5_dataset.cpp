/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_dataset.h"
#include "gth5_reference.h"
#include "gth5_node.h"
#include "gth5_file.h"

#include <QDebug>

GtH5::DataSet::DataSet() = default;

GtH5::DataSet::DataSet(std::shared_ptr<File> file, H5::DataSet dset) :
    Node{std::move(file)},
    m_dataset{std::move(dset)}
{
}

hid_t
GtH5::DataSet::id() const
{
    return m_dataset.getId();
}

H5::H5Object const*
GtH5::DataSet::toH5Object() const
{
    return &m_dataset;
}

GtH5::DataSetCProperties
GtH5::DataSet::cProperties() const
{
    try
    {
        return DataSetCProperties{m_dataset.getCreatePlist()};
    }
    catch (H5::Exception& /*e*/)
    {
        // ...
    }
    return {};
}

H5::DataSet const&
GtH5::DataSet::toH5() const
{
    return m_dataset;
}

GtH5::ObjectType
GtH5::DataSet::type() const
{
    return DataSetType;
}

bool
GtH5::DataSet::doWrite(void const* data, DataType const& dtype) const
{
    try
    {
        m_dataset.write(data, dtype.toH5());
        return true;
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Writing dataset failed! -" << name();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::doWrite failed! -"
                    << name();
    }
    return false;
}

bool
GtH5::DataSet::doRead(void* data, DataType const& dtype) const
{
    try
    {
        m_dataset.read(data, dtype.toH5());
        return true;
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Reading dataset failed! -" << name();
        return  false;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::doRead failed! -"
                    << name();
        return  false;
    }
}

bool
GtH5::DataSet::write(void const* data,
                     DataSpace const& fileSpace,
                     DataSpace const& memSpace,
                     Optional<DataType> dtype) const
{
    static constexpr auto errMsg = "HDF5: Writing data vector failed!";

    auto space = dataSpace();
    if (space.isNull())
    {
        qCritical() << errMsg << "(Null dataspace)";
        return false;
    }
    if (!data)
    {
        qCritical() << errMsg << "(Data vector is invalid)";
        return false;
    }

    auto type = dataType();
    if (dtype.isDefault() || dtype == type)
    {
        dtype = type;
    }

    try
    {
        m_dataset.write(data, dtype->toH5(), memSpace.toH5(), fileSpace.toH5());
        return true;
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Writing selection failed! ";
        return  false;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::write failed!";
        return  false;
    }
}

bool
GtH5::DataSet::read(void* data,
                    DataSpace const& fileSpace,
                    DataSpace const& memSpace,
                    Optional<DataType> dtype)
{
    static constexpr auto errMsg = "HDF5: Reading data vector failed!";

    auto space = dataSpace();
    if (space.isNull())
    {
        qCritical() << errMsg << "(Null dataspace)";
        return false;
    }
    if (!data)
    {
        qCritical() << errMsg << "(Data vector is invalid)";
        return false;
    }

    auto type = dataType();
    if (dtype.isDefault() || dtype == type)
    {
        dtype = type;
    }

    try
    {
        m_dataset.read(data, dtype->toH5(), memSpace.toH5(), fileSpace.toH5());
        return true;
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Reading selection failed! ";
        return  false;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::read failed!";
        return  false;
    }
}

H5::AbstractDs const&
GtH5::DataSet::toH5AbsDataSet() const
{
    return m_dataset;
}

bool
GtH5::DataSet::deleteLink()
{
    qDebug() << "HDF5: Deleting dataset...";

    if (!isValid())
    {
        qWarning() << "HDF5: Dataset deletion failed! (dataset is invalid)";
        return false;
    }

    auto m_dataspace = dataSpace();
    auto dims = m_dataspace.dimensions();
    dims.fill(0);

    // resize dataset to 0
    resize(dims);

    // returns error type
    if (H5Ldelete(m_file->id(), name().constData(), H5P_DEFAULT))
    {
        qCritical() << "HDF5: Dataset deletion failed!";
        return false;
    }

    close();
    return true;
}

bool
GtH5::DataSet::resize(Dimensions const& dimensions)
{    
    // dataset must be chunked
    if (!properties().isChunked())
    {
        qCritical() << "HDF5: Resizing dataset failed! (not chunked)";
        return false;
    }

    // nDims must be equal
    if (dimensions.length() != dataSpace().nDims())
    {
        qCritical() << "HDF5: Resizing dataset failed! (n-dims mismatch)";
        return false;
    }

    // check if resizing is necessary
    if (dimensions == dataSpace().dimensions())
    {
        return true;
    }

    // try resizing
    try
    {
        m_dataset.extend(dimensions.constData());
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Resizing dataset failed! ";
        return false;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::resize";
        return false;
    }
    return true;
}

void
GtH5::DataSet::close()
{
    m_dataset.close();
}
