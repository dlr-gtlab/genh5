/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_reference.h"
#include "gth5_file.h"
#include "gth5_attribute.h"
#include "gth5_group.h"
#include "gth5_dataset.h"

#include <QDebug>

GtH5::Reference::Reference() = default;

GtH5::Reference::Reference(Alignment data, ObjectType type) :
    m_type{type}
{
    m_ref.u.align = data;
}

GtH5::Reference::Reference(QByteArray buffer, ObjectType type) :
    m_type{type}
{
    if (buffer.size() != bufferSize)
    {
        qCritical().nospace()
                << "HDF5: invalid buffer format! ("
                << buffer.size() << " vs. " << bufferSize
                << "expected elements)";
        return;
    }

    std::move(std::begin(buffer), std::end(buffer), std::begin(m_ref.u.__data));
}

GtH5::Reference::Reference(H5R_ref_t ref, ObjectType type) :
    m_ref{ref},
    m_type{type}
{

}

GtH5::Reference::Reference(Location const& location) :
    m_type(location.type())
{
    m_ref.u.align = 0;

    if (!location.isValid())
    {
        qCritical() << "HDF5: Referencing location failed! "
                       "(location is invalid)";
        return;
    }

    herr_t error;

    if (location.type() == ObjectType::AttributeType)
    {
        error = H5Rcreate_attr(location.file()->id(), location.path(),
                               location.name(), H5P_DEFAULT, &m_ref);
    }
    else
    {
        error = H5Rcreate_object(location.file()->id(), location.path(),
                                 H5P_DEFAULT, &m_ref);
    }

    if (error < 0)
    {
        qCritical() << "HDF5: Referencing location (attribute) failed!";
        return;
    }

    // the file ref counter is incremented when creating a reference, however
    // this is not necessary. Therefore we decrement the ref count
    if (isValid())
    {
        H5Idec_ref(location.file()->id());
    }
}

bool
GtH5::Reference::isValid() const
{
    return alignment() > 0;
}

QByteArray
GtH5::Reference::buffer() const
{
    return {reinterpret_cast<char const*>(m_ref.u.__data), bufferSize};
}

GtH5::ObjectType
GtH5::Reference::type() const
{
    return m_type;
}

H5R_ref_t const&
GtH5::Reference::toH5() const
{
    return m_ref;
}

GtH5::Group
GtH5::Reference::toGroup(File const& file) const
{
    if (m_type != ObjectType::GroupType && m_type != ObjectType::UnkownType)
    {
        qCritical() << "HDF5: Dereferencing group failed! "
                       "(invalid object type)";
        return {};
    }

    H5::Group group;
    try
    {
        group.dereference(*file.root().toH5Object(), &m_ref);
    }
    catch (H5::GroupIException& /*e*/)
    {
        qCritical() << "HDF5: Dereferencing group failed!";
        return {};
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Reference::toGroup failed!";
        return {};
    }

    // access shared file in root group as the local one may not be the same
    return {file.root().file(), std::move(group)};
}

GtH5::DataSet
GtH5::Reference::toDataSet(File const& file) const
{
    if (m_type != ObjectType::DataSetType && m_type != ObjectType::UnkownType)
    {
        qCritical() << "HDF5: Dereferencing dataset failed! "
                       "(invalid object type)";
        return {};
    }

    H5::DataSet dset;
    try
    {
        dset.dereference(*file.root().toH5Object(), &m_ref);
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Dereferencing dataset failed!";
        return {};
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Reference::toDataSet failed!";
        return {};
    }

    // access shared file in root group as the local one may not be the same
    return {file.root().file(), std::move(dset)};
}

GtH5::Attribute
GtH5::Reference::toAttribute(File const& file) const
{
    if (m_type != ObjectType::AttributeType && m_type != ObjectType::UnkownType)
    {
        qCritical() << "HDF5: Dereferencing attribute failed! "
                       "(invalid object type)";
        return {};
    }

    auto ref = m_ref;
    hid_t id = H5Ropen_attr(&ref, H5P_DEFAULT, H5P_DEFAULT);

    if (id == -1)
    {
        qCritical() << "HDF5: Dereferencing attribute failed!";
        return {};
    }

//    H5::Attribute attr{id};

    // access shared file in root group as the local one may not be the same
    return {file.root().file(), id};
}

