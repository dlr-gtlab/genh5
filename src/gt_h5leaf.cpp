/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5leaf.h"

#include "gt_h5attribute.h"
#include "gt_h5dataspace.h"
#include "gt_h5datatype.h"

const H5::H5Location*
GtH5Leaf::toH5Location() const
{
    return this->toH5Object();
}

bool
GtH5Leaf::hasAttribute(const QByteArray& name) const
{
    return this->toH5Object()->attrExists(name.constData());
}

GtH5Attribute
GtH5Leaf::createAttribute(const QString& name,
                          const GtH5DataType& dtype,
                          const GtH5DataSpace& dspace) const
{
    return createAttribute(name.toUtf8(), dtype, dspace);
}

GtH5Attribute
GtH5Leaf::createAttribute(const QByteArray& name,
                          const GtH5DataType& dtype,
                          const GtH5DataSpace& dspace) const
{
    return GtH5Attribute::create(*this, name, dtype, dspace,
                                 GtH5Attribute::CreateOpen);
}

GtH5Attribute
GtH5Leaf::openAttribute(const QString& name) const
{
    return GtH5Attribute::open(*this, name.toUtf8());
}

GtH5Attribute
GtH5Leaf::openAttribute(const QByteArray& name) const
{
    return GtH5Attribute::open(*this, name);
}
