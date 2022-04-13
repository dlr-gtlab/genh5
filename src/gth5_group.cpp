/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_group.h"
#include "gth5_file.h"
#include "gth5_attribute.h"
#include "gth5_dataset.h"

#include <QDebug>


GtH5Group::GtH5Group() = default;

GtH5Group
GtH5Group::create(GtH5Group const& parent,
                  QByteArray const& name)
{
    if (!parent.isValid())
    {
        return {};
    }

    // create new group
    if (!parent.exists(name))
    {
        H5::Group group;
        try
        {
            group = parent.toH5Object()->createGroup(name.constData());
        }
        catch (H5::GroupIException& /*e*/)
        {
            qCritical() << "HDF5: Creating group failed!";
        }
        catch (H5::Exception& /*e*/)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5Group::create failed!";
        }

        return {parent.file(), group, name};
    }

    // open existing group
    return open(parent, name);
}

GtH5Group
GtH5Group::open(GtH5Group const& parent,
                QByteArray const& name)
{
    if (!parent.isValid())
    {
        return GtH5Group();
    }

    H5::Group group;
    try
    {
        group = parent.toH5Object()->openGroup(name.constData());
    }
    catch (H5::GroupIException& /*e*/)
    {
        qCritical() << "HDF5: Opening group failed!" << name;
        return GtH5Group();
    }
    catch(H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Group::open failed!" << name;
        return GtH5Group();
    }

    return {parent.file(), group, name};
}

GtH5Group::GtH5Group(GtH5File& file) :
    GtH5Node(nullptr, QByteArrayLiteral("/"))
{
    if (!file.isValid())
    {
        return;
    }

    m_file = std::make_shared<GtH5File>(file);

    try
    {
        m_group = file.toH5().openGroup(m_name.constData());
    }
    catch (H5::GroupIException& /*e*/)
    {
        qCritical() << "HDF5: Opening root group failed!";
    }
    catch(H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Group::GtH5Group root failed!";
    }
}

GtH5Group::GtH5Group(std::shared_ptr<GtH5File> file,
                     H5::Group const& group,
                     QByteArray const& name) :
    GtH5Node(std::move(file), name),
    m_group(group)
{
    if (m_name.isEmpty())
    {
        m_name = getObjectName(*this);
    }
}

//GtH5Group&
//GtH5Group::operator=(GtH5Group const& other)
//{
////    qDebug() << "GtH5Group::copy=";
//    auto tmp{other};
//    swap(tmp);
//    return *this;
//}

//GtH5Group&
//GtH5Group::operator=(GtH5Group&& other) noexcept
//{
////    qDebug() << "GtH5Group::move=";
//    swap(other);
//    return *this;
//}

int64_t
GtH5Group::id() const
{
    return m_group.getId();
}

bool
GtH5Group::deleteLink()
{
    return false;
}

GtH5Location::ObjectType
GtH5Group::type() const
{
    return GtH5Location::ObjectType::Group;
}

H5::H5Object const*
GtH5Group::toH5Object() const
{
    return &m_group;
}

H5::Group
GtH5Group::toH5() const
{
    return m_group;
}

void
GtH5Group::close()
{
    m_group.close();
}

GtH5Group
GtH5Group::createGroup(QString const& name)
{
    return createGroup(name.toUtf8());
}

GtH5Group
GtH5Group::createGroup(QByteArray const& name)
{
    return GtH5Group::create(*this, name);
}

GtH5Group
GtH5Group::openGroup(QString const& name)
{
    return openGroup(name.toUtf8());
}

GtH5Group
GtH5Group::openGroup(QByteArray const& name)
{
    return GtH5Group::open(*this, name);
}

GtH5DataSet
GtH5Group::createDataset(QString const& name,
                        GtH5DataType const& dtype,
                        GtH5DataSpace const& dspace)
{
    return createDataset(name.toUtf8(), dtype, dspace);
}

GtH5DataSet
GtH5Group::createDataset(QByteArray const& name,
                        GtH5DataType const& dtype,
                        GtH5DataSpace const& dspace)
{
    // chunk dataset by default
    auto props{GtH5DataSetProperties::autoChunk(dspace)};

    return GtH5DataSet::create(*this, name, dtype, dspace,
                               GtH5DataSetProperties{props});
}

GtH5DataSet
GtH5Group::openDataset(QString const& name)
{
    return openDataset(name.toUtf8());
}

GtH5DataSet
GtH5Group::openDataset(QByteArray const& name)
{
    if (!this->exists(name))
    {
        qWarning().nospace() << "HDF5 dataset '" << name << "' does not exist";
        return {};
    }
    return GtH5DataSet::open(*this, name);
}

//void
//GtH5Group::swap(GtH5Group& other) noexcept
//{
//    using std::swap;
//    GtH5Node::swap(other);
//    swap(m_group, other.m_group);
//}

//void
//swap(GtH5Group& first, GtH5Group& other) noexcept
//{
////    qDebug() << "swap(GtH5Group)";
//    first.swap(other);
//}
