/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_location.h"
#include "gth5_reference.h"
#include "gth5_group.h"
#include "gth5_file.h"
#include "gth5_attribute.h"

#include <QDebug>

bool
GtH5Location::isValid() const
{
    return GtH5Object::isValid() && m_file != nullptr && m_file->isValid();
}

GtH5Location::ObjectType
GtH5Location::type() const
{
    return Unkown;
}

bool
GtH5Location::exists(QByteArray const& path) const
{
    return exists(path.split('/'));
}

bool
GtH5Location::exists(QByteArrayList const& path) const
{
    QByteArray subPath;

    for (auto& entry : qAsConst(path))
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

QByteArray const&
GtH5Location::name() const
{
    return m_name;
}

GtH5Reference
GtH5Location::toReference()
{
    if (!isValid())
    {
        return {};
    }

    return GtH5Reference(*this);
}

std::shared_ptr<GtH5File>
GtH5Location::file() const
{
    return m_file;
}

GtH5Location::GtH5Location(std::shared_ptr<GtH5File> file,
                           QByteArray const& name) :
    m_file(std::move(file)),
    m_name(name)
{

}

//GtH5Location::GtH5Location(GtH5Location const& other) :
//    m_file{other.m_file},
//    m_name{other.m_name}
//{

//}

//GtH5Location::GtH5Location(GtH5Location&& other) noexcept :
//    m_file{std::move(other.m_file)},
//    m_name{std::move(other.m_name)}
//{

//}

QByteArray
GtH5Location::path() const
{
    if (!this->isValid())
    {
        return {};
    }

    size_t bufferLen = 32;
    QByteArray buffer(32, ' ');

    auto acutalLen = static_cast<size_t>(
                H5Iget_name(this->id(), buffer.data(), bufferLen));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(static_cast<int>(bufferLen));
        H5Iget_name(this->id(), buffer.data(), bufferLen);
    }

    // chop of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}


QByteArray
GtH5Location::getObjectName(GtH5Location const& location)
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
GtH5Location::getAttrName(GtH5Attribute const& attr)
{
    if (!attr.isValid())
    {
        return {};
    }

    size_t bufferLen = 32;
    QByteArray buffer{32, ' '};

    auto acutalLen = static_cast<size_t>(
                H5Aget_name(attr.id(), bufferLen, buffer.data()));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(static_cast<int>(bufferLen));
        H5Aget_name(attr.id(), bufferLen, buffer.data());
    }

    // remove of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}

//void
//GtH5Location::swap(GtH5Location& other) noexcept
//{
////    qDebug() << "GtH5Location::swap";
//    using std::swap;
//    swap(m_file, other.m_file);
//    swap(m_name, other.m_name);
//}
