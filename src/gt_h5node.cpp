/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5node.h"

#include "gt_h5group.h"
#include "gt_h5attribute.h"
#include "gt_h5dataset.h"
#include "gt_h5reference.h"

#include <QDebug>

GtH5Group
GtH5Node::createGroup(const QString& name)
{
    return createGroup(name.toUtf8());
}

GtH5Group
GtH5Node::createGroup(const QByteArray& name)
{
    return GtH5Group::create(*this, name, GtH5Group::CreateOpen);
}

GtH5Group
GtH5Node::openGroup(const QString& name)
{
    return openGroup(name.toUtf8());
}

GtH5Group
GtH5Node::openGroup(const QByteArray& name)
{
    return GtH5Group::open(*this, name);
}

GtH5DataSet
GtH5Node::createDataset(const QString& name,
                        const GtH5DataType& dtype,
                        const GtH5DataSpace& dspace)
{
    return createDataset(name.toUtf8(), dtype, dspace);
}

GtH5DataSet
GtH5Node::createDataset(const QByteArray& name,
                        const GtH5DataType& dtype,
                        const GtH5DataSpace& dspace)
{
    return GtH5DataSet::create(*this, name, dtype, dspace,
                               GtH5DataSet::CreateOpen);
}

GtH5DataSet
GtH5Node::openDataset(const QString& name)
{
    return openDataset(name.toUtf8());
}

GtH5DataSet
GtH5Node::openDataset(const QByteArray& name)
{
    return GtH5DataSet::open(*this, name);
}
