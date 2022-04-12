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


GtH5Reference::GtH5Reference() = default;

GtH5Reference::GtH5Reference(int64_t data, ObjectType type) :
    m_type(type)
{
    m_ref.u.align = data;
}

GtH5Reference::GtH5Reference(H5R_ref_t const& ref, ObjectType type) :
    m_ref(ref),
    m_type(type)
{

}

GtH5Reference::GtH5Reference(GtH5Location const& location) :
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

    if (location.type() == ObjectType::Attribute)
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
    if (this->isValid())
    {
        H5Idec_ref(location.file()->id());
    }
}

bool
GtH5Reference::isValid() const
{
    return data() > 0;
}

int64_t
GtH5Reference::data() const
{
    return m_ref.u.align;
}

GtH5Reference::ObjectType
GtH5Reference::type() const
{
    return m_type;
}

H5R_ref_t
GtH5Reference::toH5() const
{
    return m_ref;
}

GtH5Group
GtH5Reference::toGroup(GtH5File const& file) const
{
    if (m_type != ObjectType::Group && m_type != ObjectType::Unkown)
    {
        qCritical() << "HDF5: Derefernecing group failed! "
                       "(invalid object type)";
        return GtH5Group();
    }

    H5::Group group;
    try
    {
        group.dereference(*file.root().toH5Object(), &m_ref);
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Derefernecing group failed!";
        return GtH5Group();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Reference::toGroup failed!";
        return GtH5Group();
    }

    // access shared file in root group as the local one may not be the same
    return GtH5Group(file.root().file(), group);
}

GtH5DataSet
GtH5Reference::toDataSet(GtH5File const& file) const
{
    if (m_type != ObjectType::DataSet && m_type != ObjectType::Unkown)
    {
        qCritical() << "HDF5: Derefernecing dataset failed! "
                       "(invalid object type)";
        return GtH5DataSet();
    }

    H5::DataSet dset;
    try
    {
        dset.dereference(*file.root().toH5Object(), &m_ref);
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Derefernecing dataset failed!";
        return GtH5DataSet();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Reference::toDataSet failed!";
        return GtH5DataSet();
    }

    // access shared file in root group as the local one may not be the same
    return GtH5DataSet(file.root().file(), dset);
}

GtH5Attribute
GtH5Reference::toAttribute(GtH5File  const& file) const
{
    if (m_type != ObjectType::Attribute && m_type != ObjectType::Unkown)
    {
        qCritical() << "HDF5: Derefernecing attribute failed! "
                       "(invalid object type)";
        return GtH5Attribute();
    }

    auto ref = m_ref;
    hid_t id = H5Ropen_attr(&ref, H5P_DEFAULT, H5P_DEFAULT);

    if (id == -1)
    {
        qCritical() << "HDF5: Derefernecing attribute failed!";
        return GtH5Attribute();
    }

    H5::Attribute attr(id);

    // access shared file in root group as the local one may not be the same
    return GtH5Attribute(file.root().file(), attr);
}

