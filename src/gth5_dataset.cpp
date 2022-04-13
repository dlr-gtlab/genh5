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


GtH5DataSet::GtH5DataSet() = default;

GtH5DataSet
GtH5DataSet::create(GtH5Group const& parent,
                    QByteArray const& name,
                    GtH5DataType const& dtype,
                    GtH5DataSpace const& dspace,
                    GtH5DataSetProperties const& properties)
{
    if (!parent.isValid())
    {
        return GtH5DataSet();
    }

    // create new dataset
    if (!parent.exists(name))
    {
        H5::DataSet dset;
        try
        {
            dset = parent.toH5Object()->createDataSet(name.constData(),
                                                      dtype.toH5(),
                                                      dspace.toH5(),
                                                      properties.toH5());
        }
        catch (H5::DataSetIException& /*e*/)
        {
            qCritical() << "HDF5: Creating dataset failed! -" << name;
            return GtH5DataSet();
        }
        catch (H5::Exception& /*e*/)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::create failed! -"
                        << name;
            return GtH5DataSet();
        }

        return GtH5DataSet(parent.file(), dset, name);
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

    // dataset cannot be resized and must be deleted
    qWarning() << "HDF5: Invalid memory layout! Overwriting dataset! -" << name;
    if (!dset.deleteLink())
    {
        return {};
    }

    return create(parent, name, dtype, dspace, properties);
}

GtH5DataSet
GtH5DataSet::open(GtH5Group const& parent, QByteArray const& name)
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
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Opening dataset failed! -" << name;
        return GtH5DataSet();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::open failed! -" << name;
        return GtH5DataSet();
    }

    return GtH5DataSet(parent.file(), dset, name);
}

GtH5DataSet::GtH5DataSet(std::shared_ptr<GtH5File> file,
                         H5::DataSet const& dset,
                         QByteArray const& name) :
    GtH5Node(std::move(file), name),
    GtH5AbtsractDataSet{GtH5DataType{dset.getDataType()},
                        GtH5DataSpace{dset.getSpace()}},
    m_dataset(dset),
    m_properties(m_dataset.getCreatePlist())
{
    if (m_name.isEmpty())
    {
        m_name = getObjectName(*this);
    }
}

//GtH5DataSet::GtH5DataSet(GtH5DataSet const& other) :
//    GtH5Node(other),
//    GtH5AbtsractDataSet(other),
//    m_dataset{other.m_dataset},
//    m_properties{other.m_properties}
//{
////    qDebug() << "GtH5DataSet::copy";
//}

//GtH5DataSet::GtH5DataSet(GtH5DataSet&& other) noexcept :
//    GtH5Node(std::move(other)),
//    GtH5AbtsractDataSet(std::move(other)),
//    m_dataset{std::move(other.m_dataset)},
//    m_properties{std::move(other.m_properties)}
//{
////    qDebug() << "GtH5DataSet::move";
//}

//GtH5DataSet&
//GtH5DataSet::operator=(GtH5DataSet const& other)
//{
////    qDebug() << "GtH5DataSet::copy=";
//    auto dset{other};
//    swap(dset);
//    return *this;
//}

//GtH5DataSet&
//GtH5DataSet::operator=(GtH5DataSet&& other)  noexcept
//{
////    qDebug() << "GtH5DataSet::move=";
//    swap(other);
//    return *this;
//}

int64_t
GtH5DataSet::id() const
{
    return m_dataset.getId();
}

H5::H5Object const*
GtH5DataSet::toH5Object() const
{
    return &m_dataset;
}

bool
GtH5DataSet::isValid() const
{
    return GtH5Node::isValid() && m_datatype.isValid() &&
           m_dataspace.isValid();
}

GtH5DataSetProperties const&
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
GtH5DataSet::doWrite(void const* data) const
{
    try
    {
        m_dataset.write(data, m_datatype.toH5());
        return true;
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Writing dataset failed! -" << m_name;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::doWrite failed! -"
                    << m_name;
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
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Reading dataset failed! ";
        return  false;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSet::doRead failed!";
        return  false;
    }
}

bool
GtH5DataSet::deleteLink()
{
    qDebug() << "HDF5: Deleting dataset...";

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
        m_dataspace = GtH5DataSpace(m_dataset.getSpace());
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Resizing dataset failed! ";
        return false;
    }
    catch (H5::Exception& /*e*/)
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

//void
//GtH5DataSet::swap(GtH5DataSet& other) noexcept
//{
//    using std::swap;
//    GtH5Node::swap(other);
//    GtH5AbtsractDataSet::swap(other);
//    swap(m_dataset, other.m_dataset);
//    swap(m_properties, other.m_properties);
//}

//void
//swap(GtH5DataSet& first, GtH5DataSet& other) noexcept
//{
////    qDebug() << "swap(GtH5DataSet)";
//    first.swap(other);
//}

