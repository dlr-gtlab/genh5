/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5node.h"

#include "gt_h5attribute.h"
#include "gt_h5dataspace.h"
#include "gt_h5datatype.h"


GtH5Node::GtH5Node(std::shared_ptr<GtH5File> file, QByteArray const& name) :
    GtH5Location(std::move(file), name)
{

}

const H5::H5Location*
GtH5Node::toH5Location() const
{
    return this->toH5Object();
}

bool
GtH5Node::hasAttribute(QByteArray const& name) const
{
    return this->toH5Object()->attrExists(name.constData());
}

GtH5Attribute
GtH5Node::createAttribute(QString const& name,
                              GtH5DataType const& dtype,
                              GtH5DataSpace const& dspace) const
{
    return createAttribute(name.toUtf8(), dtype, dspace);
}

GtH5Attribute
GtH5Node::createAttribute(QByteArray const& name,
                              GtH5DataType const& dtype,
                              GtH5DataSpace const& dspace) const
{
    return GtH5Attribute::create(*this, name, dtype, dspace);
}

GtH5Attribute
GtH5Node::openAttribute(QString const& name) const
{
    return GtH5Attribute::open(*this, name.toUtf8());
}

GtH5Attribute
GtH5Node::openAttribute(QByteArray const& name) const
{
    return GtH5Attribute::open(*this, name);
}
