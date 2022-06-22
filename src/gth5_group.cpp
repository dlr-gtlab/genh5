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


GtH5::Group::Group() = default;

GtH5::Group::Group(const File& file) :
    Node{nullptr}
{
    if (!file.isValid())
    {
        return;
    }

    m_file = std::make_shared<File>(file);

    try
    {
        m_group = file.toH5().openGroup(QByteArrayLiteral("/").constData());
    }
    catch (H5::GroupIException& /*e*/)
    {
        qCritical() << "HDF5: Opening root group failed!";
    }
    catch(H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Group::GtH5Group root failed!";
    }
}

GtH5::Group::Group(std::shared_ptr<File> file, H5::Group group) :
    Node{std::move(file)},
    m_group{std::move(group)}
{ }

hid_t
GtH5::Group::id() const
{
    return m_group.getId();
}

bool
GtH5::Group::deleteLink()
{
    return false;
}

GtH5::ObjectType
GtH5::Group::type() const
{
    return GroupType;
}

H5::H5Object const*
GtH5::Group::toH5Object() const
{
    return &m_group;
}

H5::Group const&
GtH5::Group::toH5() const
{
    return m_group;
}

void
GtH5::Group::close()
{
    m_group.close();
}

GtH5::Group
GtH5::Group::createGroup(QString const& name)
{
    return createGroup(name.toUtf8());
}

GtH5::Group
GtH5::Group::createGroup(String const& name)
{
    auto const& parent = *this;

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
            qCritical() << "HDF5: [EXCEPTION] GtH5::Group::create failed!";
        }

        return {parent.file(), std::move(group)};
    }

    // open existing group
    return openGroup(std::move(name));
}

GtH5::Group
GtH5::Group::openGroup(QString const& name)
{
    return openGroup(name.toUtf8());
}

GtH5::Group
GtH5::Group::openGroup(String const& name)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        return {};
    }

    H5::Group group;
    try
    {
        group = parent.toH5Object()->openGroup(name.constData());
    }
    catch (H5::GroupIException& /*e*/)
    {
        qCritical() << "HDF5: Opening group failed!" << name;
        return Group();
    }
    catch(H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Group::open failed!" << name;
        return Group();
    }

    return {parent.file(), std::move(group)};
}

GtH5::DataSet
GtH5::Group::createDataset(QString const& name,
                           DataType const& dtype,
                           DataSpace const& dspace,
                           Optional<DataSetCProperties> properties)
{
    return createDataset(name.toUtf8(), dtype, dspace, std::move(properties));
}

GtH5::DataSet
GtH5::Group::createDataset(String const& name,
                           DataType const& dtype,
                           DataSpace const& dspace,
                           Optional<DataSetCProperties> properties)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        return {};
    }

    // chunk dataset by default
    if (properties.isDefault() && !dspace.isScalar() && dspace.size() > 0)
    {
        properties = DataSetCProperties::autoChunked(dspace);
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
                                                      properties->toH5());
        }
        catch (H5::DataSetIException& /*e*/)
        {
            qCritical() << "HDF5: Creating dataset failed! -" << name;
            return {};
        }
        catch (H5::Exception& /*e*/)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::create failed! -"
                        << name;
            return {};
        }

        DataSetCProperties props{dset.getCreatePlist()};

        return {parent.file(), std::move(dset)};
    }

    // open existing dataset
    auto dset = openDataset(name);

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

    return createDataset(name, dtype, dspace, std::move(properties));
}

GtH5::DataSet
GtH5::Group::openDataset(QString const& name)
{
    return openDataset(name.toUtf8());
}

GtH5::DataSet
GtH5::Group::openDataset(String const& name)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        return {};
    }

    H5::DataSet dset;
    try
    {
        dset = parent.toH5Object()->openDataSet(name.constData());
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Opening dataset failed! -" << name;
        return {};
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSet::open failed! -"
                    << name;
        return {};
    }

    return {parent.file(), std::move(dset)};
}
