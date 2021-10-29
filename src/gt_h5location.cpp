/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5location.h"
#include "gt_h5reference.h"
#include "gt_h5group.h"
#include "gt_h5file.h"
#include "gt_h5attribute.h"

#include <QDebug>

QByteArray
GtH5Location::path() const
{
    if (!this->isValid())
    {
        return QByteArray();
    }

    size_t bufferLen = 32;
    QByteArray buffer(32, ' ');

    size_t acutalLen = H5Iget_name(this->id(), buffer.data(), bufferLen);

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(bufferLen);
        H5Iget_name(this->id(), buffer.data(), bufferLen);
    }

    // chop of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}

bool
GtH5Location::isValid() const
{
    return GtH5Object::isValid() && m_file != Q_NULLPTR;
}

GtH5Location::ObjectType
GtH5Location::type() const
{
    return Unkown;
}

bool
GtH5Location::exists(const QByteArray& path) const
{
    return exists(QByteArrayList(path.split('/')));
}

bool
GtH5Location::exists(const QByteArrayList& path) const
{
    QByteArray subPath;

    foreach (QByteArray entry, path)
    {
        subPath.append(entry);
        subPath.append('/');

        if (!this->toH5Location()->nameExists(subPath))
        {
            return false;
        }
    }

    return true;
}

QByteArray GtH5Location::name() const
{
    return m_name;
}

GtH5Reference
GtH5Location::toReference()
{
    if (!isValid())
    {
        return GtH5Reference();
    }

    return GtH5Reference(*this);
}

QByteArray
GtH5Location::getObjectName(GtH5Location& location)
{
    QByteArrayList path = location.path().split('/');

    // should not be the case
    if (path.isEmpty())
    {
        qWarning() << "HDF5: Failed to retrieve the location name!";
        return QByteArrayLiteral("<-unkown->");
    }

    return path.last();
}

QByteArray
GtH5Location::getAttrName(GtH5Attribute& attr)
{
    if (!attr.isValid())
    {
        return QByteArray();
    }

    size_t bufferLen = 32;
    QByteArray buffer(32, ' ');

    size_t acutalLen = H5Aget_name(attr.id(), bufferLen, buffer.data());

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(bufferLen);
        H5Aget_name(attr.id(), bufferLen, buffer.data());
    }

    // chop of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}
