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
        throw GroupException{"Opening root group failed! (Invalid file)"};
    }

    m_file = std::make_shared<File>(file);

    try
    {
        m_group = file.toH5().openGroup(QByteArrayLiteral("/").constData());
    }
    catch (H5::GroupIException const& e)
    {
        throw GroupException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Group::Group";
        throw GroupException{e.getCDetailMsg()};
    }
}

GtH5::Group::Group(std::shared_ptr<File> file, H5::Group group) :
    Node{std::move(file)},
    m_group{std::move(group)}
{ }

hid_t
GtH5::Group::id() const noexcept
{
    return m_group.getId();
}

void
GtH5::Group::deleteLink() noexcept(false)
{    
    // returns error type
    if (H5Ldelete(m_file->id(), name().constData(), H5P_DEFAULT))
    {
        throw LocationException{"Deleting group failed"};
    }
    close();
}

H5::H5Object const*
GtH5::Group::toH5Object() const noexcept
{
    return &m_group;
}

H5::Group const&
GtH5::Group::toH5() const noexcept
{
    return m_group;
}

void
GtH5::Group::close()
{
    m_group.close();
}

GtH5::Group
GtH5::Group::createGroup(QString const& name) noexcept(false)
{
    return createGroup(name.toUtf8());
}

GtH5::Group
GtH5::Group::createGroup(String const& name) noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw GroupException{"Creating group failed (Invalid parent"};
    }

    // create new group
    if (!parent.exists(name))
    {
        try
        {
            H5::Group group = parent.toH5().createGroup(name.constData());
            return {parent.file(), std::move(group)};
        }
        catch (H5::GroupIException const& e)
        {
            throw GroupException{e.getCDetailMsg()};
        }
        catch (H5::Exception const& e)
        {
            qCritical() << "HDF5: [EXCEPTION] Group::createGroup";
            throw GroupException{e.getCDetailMsg()};
        }
    }

    // open existing group
    return openGroup(std::move(name));
}

GtH5::Group
GtH5::Group::openGroup(QString const& name) noexcept(false)
{
    return openGroup(name.toUtf8());
}

GtH5::Group
GtH5::Group::openGroup(String const& name) noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw GroupException{"Opening group failed (Invalid parent"};
    }

    try
    {
        H5::Group group = parent.toH5().openGroup(name.constData());
        return {parent.file(), std::move(group)};
    }
    catch (H5::GroupIException const& e)
    {
        throw GroupException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Group::openGroup";
        throw GroupException{e.getCDetailMsg()};
    }
}

GtH5::DataSet
GtH5::Group::createDataset(QString const& name,
                           DataType const& dtype,
                           DataSpace const& dspace,
                           Optional<DataSetCProperties> properties
                           ) noexcept(false)
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
        throw DataSetException{"Opening dataset failed (Invalid parent"};
    }

    // chunk dataset by default
    if (properties.isDefault() && !dspace.isScalar() && dspace.size() > 0)
    {
        properties = DataSetCProperties::autoChunked(dspace);
    }

    // create new dataset
    if (!parent.exists(name))
    {
        try
        {
            H5::DataSet dset = parent.toH5().createDataSet(name.constData(),
                                                           dtype.toH5(),
                                                           dspace.toH5(),
                                                           properties->toH5());
            return {parent.file(), std::move(dset)};
        }
        catch (H5::DataSetIException const& e)
        {
            throw DataSetException{e.getCDetailMsg()};
        }
        catch (H5::Exception const& e)
        {
            qCritical() << "HDF5: [EXCEPTION] Group::createDataset";
            throw DataSetException{e.getCDetailMsg()};
        }
    }

    // open existing dataset
    auto dset = openDataset(name);

    // check if datatype is equal
    if (dset.dataType() == dtype)
    {
        // check if dataspace is equal or resize dataset
        if (dset.dataSpace() == dspace || dset.resize(dspace.dimensions()))
        {
            return dset;
        }
    }

    // dataset cannot be resized and must be deleted
    qWarning() << "HDF5: Invalid memory layout! Overwriting dataset! -" << name;
    dset.deleteLink();

    return createDataset(name, dtype, dspace, std::move(properties));
}

GtH5::DataSet
GtH5::Group::openDataset(QString const& name) noexcept(false)
{
    return openDataset(name.toUtf8());
}

GtH5::DataSet
GtH5::Group::openDataset(String const& name) noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw DataSetException{"Opening dataset failed (Invalid parent)"};
    }

    try
    {
        H5::DataSet dset = parent.toH5().openDataSet(name.constData());
        return {parent.file(), std::move(dset)};
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::GroupIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Group::openDataset";
        throw DataSetException{e.getCDetailMsg()};
    }
}
