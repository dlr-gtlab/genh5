/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5reference.h"
#include "gt_h5file.h"
#include "gt_h5attribute.h"
#include "gt_h5group.h"
#include "gt_h5dataset.h"

#include <QDebug>


GtH5Reference::GtH5Reference() :
    m_type(ObjectType::Unkown)
{
    m_ref.u.align = 0;
}

GtH5Reference::GtH5Reference(int64_t data, ObjectType type) :
    m_type(type)
{
    m_ref.u.align = data;
}

GtH5Reference::GtH5Reference(const H5R_ref_t& ref, ObjectType type) :
    m_ref(ref),
    m_type(type)
{
}

GtH5Reference::GtH5Reference(const GtH5Location& location) :
    m_type(location.type())
{
    m_ref.u.align = 0;

    if (!location.isValid())
    {
        qCritical() << "HDF5: Referencing location failed! (location is invalid)";
        return;
    }

    herr_t error;

    if (location.type() == ObjectType::Attribute)
    {
        error = H5Rcreate_attr(location.file()->root().id(), location.path(),
                               location.name(), H5P_DEFAULT, &m_ref);
    }
    else
    {
        error = H5Rcreate_object(location.file()->root().id(), location.path(),
                                 H5P_DEFAULT, &m_ref);
    }

    if (error < 0)
    {
        qCritical() << "HDF5: Referencing location (attribute) failed!";
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
GtH5Reference::toGroup(GtH5File& file) const
{
    if (m_type != ObjectType::Group && m_type != ObjectType::Unkown)
    {
        qCritical() << "HDF5: Derefernecing group failed! (invalid object type)";
        return GtH5Group();
    }

    H5::Group group;
    try
    {
        group.dereference(*file.root().toH5Object(), &m_ref);
    }
    catch (H5::DataSetIException e)
    {
        qCritical() << "HDF5: Derefernecing group failed!";
        return GtH5Group();
    }
    catch (H5::Exception e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Reference::toGroup failed!";
        return GtH5Group();
    }

    return GtH5Group(file, group);
}

GtH5DataSet
GtH5Reference::toDataSet(GtH5File& file) const
{
    if (m_type != ObjectType::DataSet && m_type != ObjectType::Unkown)
    {
        qCritical() << "HDF5: Derefernecing dataset failed! (invalid object type)";
        return GtH5DataSet();
    }

    H5::DataSet dset;
    try
    {
        dset.dereference(*file.root().toH5Object(), &m_ref);
    }
    catch (H5::DataSetIException e)
    {
        qCritical() << "HDF5: Derefernecing dataset failed!";
        return GtH5DataSet();
    }
    catch (H5::Exception e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Reference::toDataSet failed!";
        return GtH5DataSet();
    }

    return GtH5DataSet(file, dset);
}

GtH5Attribute
GtH5Reference::toAttribute(GtH5File& file) const
{
    if (m_type != ObjectType::Attribute && m_type != ObjectType::Unkown)
    {
        qCritical() << "HDF5: Derefernecing attribute failed! (invalid object type)";
        return GtH5Attribute();
    }

    hid_t id = H5Ropen_attr(const_cast<H5R_ref_t*>(&m_ref),
                            H5P_DEFAULT, H5P_DEFAULT);

    if (id == -1)
    {
        qCritical() << "HDF5: Derefernecing attribute failed!";
        return GtH5Attribute();
    }

    H5::Attribute attr(id);

    return GtH5Attribute(file, attr);
}
