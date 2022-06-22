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

GtH5::String
GtH5::getObjectName(Location const& location)
{
    auto path = location.path().split('/');

    // should not be the case
    if (path.isEmpty())
    {
        qWarning() << "HDF5: Failed to retrieve the location name!";
        return QByteArrayLiteral("<-unkown->");
    }

    return path.last();
}

bool
GtH5::Location::isValid() const
{
    return Object::isValid() && m_file && m_file->isValid();
}

GtH5::ObjectType
GtH5::Location::type() const
{
    return UnkownType;
}

bool
GtH5::Location::exists(String const& path) const
{
    return exists(path.split('/').toVector());
}

bool
GtH5::Location::exists(Vector<String> const& path) const
{
    String subPath{};

    for (auto& entry : qAsConst(path))
    {
        subPath.push_back(entry);
        subPath.push_back('/');

        if (!toH5Location()->nameExists(subPath))
        {
            return false;
        }
    }

    return true;
}

GtH5::String
GtH5::Location::name() const
{
    return getObjectName(*this);
}

GtH5::Reference
GtH5::Location::toReference()
{
    if (!isValid())
    {
        return {};
    }

    return GtH5::Reference(*this);
}

GtH5::Location::Location(std::shared_ptr<File> file) :
    m_file(std::move(file))
{ }

GtH5::String
GtH5::Location::path() const
{
    if (!isValid())
    {
        return {};
    }

    String buffer{32, ' '};
    size_t bufferLen = static_cast<size_t>(buffer.size());

    auto acutalLen = static_cast<size_t>(H5Iget_name(id(), buffer.data(),
                                                     bufferLen));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(static_cast<int>(bufferLen));
        H5Iget_name(id(), buffer.data(), bufferLen);
    }

    // chop of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}
