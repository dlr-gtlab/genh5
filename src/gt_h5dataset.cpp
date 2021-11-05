/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5dataset.h"
#include "gt_h5reference.h"
#include "gt_h5node.h"
#include "gt_h5file.h"

#include <QDebug>

GtH5DataSet
GtH5DataSet::create(const GtH5Node& parent,
                    const QByteArray& name,
                    const GtH5DataType& dtype,
                    const GtH5DataSpace& dspace,
                    AccessFlag flag)
{
    GtH5DataSetProperties props  = GtH5DataSetProperties::autoChunk(dspace);
    return create(parent, name, dtype, dspace, props, flag);
}

GtH5DataSet
GtH5DataSet::create(const GtH5Node& parent,
                    const QByteArray& name,
                    const GtH5DataType& dtype,
                    const GtH5DataSpace& dspace,
                    const GtH5DataSetProperties& properties,
                    AccessFlag flag)
{
    if (!parent.isValid())
    {
        return GtH5DataSet();
    }

    // create new dataset
    if (flag == AccessFlag::Create || !parent.exists(name))
    {
        H5::DataSet dset;
        try
        {
            dset = parent.toH5Object()->createDataSet(name.constData(),
                                                      dtype.toH5(),
                                                      dspace.toH5(),
                                                      properties.toH5());
        }
        catch (H5::DataSetIException& e)
        {
            qCritical() << "HDF5: Creating dataset failed!";
            return GtH5DataSet();
        }
        catch (H5::Exception& e)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::create failed!";
            return GtH5DataSet();
        }

        return GtH5DataSet(*parent.file(), dset, name);
    }

    // open existing dataset
    GtH5DataSet dset = open(parent, name);

    // check if datatype is equal
    if (dset.dataType() == dtype)
    {
        // check if dataspace is equal
        if (dset.dataSpace() == dspace)
        {
            return dset;
        }

        // try resizing the existing dataset
        if (dset.resize(dspace.dimensions()))
        {
            return dset;
        }
    }

    // dataset must be deleted as datatype or dataspace does not align!
    if (flag != CreateOverwrite)
    {
        qCritical() << "HDF5: Overwriting dataset failed! "
                       "(invalid memory layout)";
        return GtH5DataSet();
    }

    dset.deleteLink();

    return create(parent, name, dtype, dspace, properties, Create);
}

GtH5DataSet
GtH5DataSet::open(const GtH5Node& parent, const QByteArray& name)
{
    if (!parent.isValid())
    {
        return GtH5DataSet();
    }

    H5::DataSet dset;
    try
    {
        dset = parent.toH5Object()->openDataSet(name.constData());
    }
    catch (H5::DataSetIException& e)
    {
        qCritical() << "HDF5: Opening dataset failed!";
        return GtH5DataSet();
    }
    catch (H5::Exception& e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::open failed!";
        return GtH5DataSet();
    }

    return GtH5DataSet(*parent.file(), dset, name);
}

GtH5DataSet::GtH5DataSet(GtH5File& file,
                         const H5::DataSet& dset,
                         const QByteArray& name) :
    m_dataset(dset),
    m_properties(m_dataset.getCreatePlist())
{
    m_datatype  = GtH5DataType(dset.getDataType());
    m_dataspace = GtH5DataSpace(dset.getSpace());
    m_file = &file;
    m_name = name.isEmpty() ? getObjectName(*this) : name;
}

int64_t
GtH5DataSet::id() const
{
    return m_dataset.getId();
}

const H5::H5Object*
GtH5DataSet::toH5Object() const
{
    return &m_dataset;
}

bool
GtH5DataSet::isValid() const
{
    return GtH5Leaf::isValid() && m_datatype.isValid() && m_dataspace.isValid();
}

GtH5DataSetProperties
GtH5DataSet::properties() const
{
    return m_properties;
}

H5::DataSet
GtH5DataSet::toH5() const
{
    return m_dataset;
}

GtH5Location::ObjectType
GtH5DataSet::type() const
{
    return GtH5Location::ObjectType::DataSet;
}

bool
GtH5DataSet::doWrite(void* data) const
{
    try
    {
        m_dataset.write(data, m_datatype.toH5());
        return true;
    }
    catch (H5::DataSetIException& e)
    {
        qCritical() << "HDF5: Writing dataset failed!";
    }
    catch (H5::Exception& e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::doWrite failed!";
    }
    return false;
}

bool
GtH5DataSet::doRead(void* data) const
{
    try
    {
        m_dataset.read(data, m_datatype.toH5());
        return true;
    }
    catch (H5::DataSetIException& e)
    {
        qCritical() << "HDF5: Reading dataset failed! ";
        return  false;
    }
    catch (H5::Exception& e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::doRead failed!";
        return  false;
    }
}

bool
GtH5DataSet::deleteLink()
{
    qDebug() << "HDF5: GtH5DataSet::delete";

    if (!this->isValid())
    {
        qWarning() << "HDF5: Dataset deletion failed! (dataset is invalid)";
        return false;
    }

    QVector<uint64_t> dims(m_dataspace.dimensions());
    dims.fill(0);

    // resize dataset to 0
    resize(dims);

    // returns error type
    if (H5Ldelete(m_file->id(), m_name.constData(), H5P_DEFAULT))
    {
        qCritical() << "HDF5: Dataset deletion failed!";
        return false;
    }

    this->close();
    return true;
}

bool
GtH5DataSet::resize(const QVector<uint64_t>& dimensions)
{    
    // dataset must be chunked
    if (!m_properties.isChunked())
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
        m_dataset.extend(
                    reinterpret_cast<const hsize_t*>(dimensions.constData()));
        // update dataspace
        m_dataspace = m_dataset.getSpace();
    }
    catch (H5::DataSetIException& e)
    {
        qCritical() << "HDF5: Resizing dataset failed! ";
        return false;
    }
    catch (H5::Exception& e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::resize";
        return false;
    }
    return true;
}

void
GtH5DataSet::close()
{
    m_dataset.close();
}
