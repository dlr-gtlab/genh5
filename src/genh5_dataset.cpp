/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_dataset.h"
#include "genh5_reference.h"
#include "genh5_node.h"
#include "genh5_file.h"

#include <QDebug>

GenH5::DataSet::DataSet() = default;

GenH5::DataSet::DataSet(std::shared_ptr<File> file, H5::DataSet dset) :
    Node{std::move(file)},
    m_dataset{std::move(dset)}
{ }

hid_t
GenH5::DataSet::id() const noexcept
{
    return m_dataset.getId();
}

H5::H5Object const*
GenH5::DataSet::toH5Object() const noexcept
{
    return &m_dataset;
}

GenH5::DataSetCProperties
GenH5::DataSet::cProperties() const noexcept(false)
{
    try
    {
        return DataSetCProperties{m_dataset.getCreatePlist()};
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataSet::cProperties";
        throw DataSetException{e.getCDetailMsg()};
    }
}

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
H5::DataSet const&
GenH5::DataSet::toH5() const noexcept
{
    return m_dataset;
}
#endif

bool
GenH5::DataSet::doWrite(void const* data, DataType const& dtype) const
{
    try
    {
        m_dataset.write(data, dtype.toH5());
        return true;
    }
    catch (H5::DataSetIException const& e)
    {
        qCritical() << "HDF5: Writing dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Writing dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
}

bool
GenH5::DataSet::doRead(void* data, DataType const& dtype) const
{
    try
    {
        m_dataset.read(data, dtype.toH5());
        return true;
    }
    catch (H5::DataSetIException const& e)
    {
        qCritical() << "HDF5: Reading dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Reading dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
}

bool
GenH5::DataSet::write(void const* data,
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
    catch (H5::DataSetIException const& e)
    {
        qCritical() << "HDF5: Writing dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Writing dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
}

bool
GenH5::DataSet::read(void* data,
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
    catch (H5::DataSetIException const& e)
    {
        qCritical() << "HDF5: Reading dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Reading dataset failed! -" << name();
        throw DataSetException{e.getCDetailMsg()};
    }
}

H5::AbstractDs const&
GenH5::DataSet::toH5AbsDataSet() const noexcept
{
    return m_dataset;
}

void
GenH5::DataSet::deleteLink() noexcept(false)
{
    qDebug() << "HDF5: Deleting dataset..." << name();

    if (!isValid())
    {
        throw LocationException{"Deleting dataset failed (Invalid dataset)"};
    }

    auto m_dataspace = dataSpace();
    auto dims = m_dataspace.dimensions();
    dims.fill(0);

    // resize dataset to 0
    resize(dims);

    // returns error type
    if (H5Ldelete(m_file->id(), path().constData(), H5P_DEFAULT) < 0)
    {
        throw LocationException{"Deleting dataset failed"};
    }
    close();
}

void
GenH5::DataSet::deleteRecursively() noexcept(false)
{
    // attributes
    for (auto& attr : findAttributes())
    {
        attr.toAttribute(*this).deleteLink();
    }

    // this
    deleteLink();
}

bool
GenH5::DataSet::resize(Dimensions const& dimensions) noexcept(false)
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
    if (dimensions != dataSpace().dimensions())
    {
        m_dataset.extend(dimensions.constData());
    }
    return true;
}

void
GenH5::DataSet::close()
{
    m_dataset.close();
}
