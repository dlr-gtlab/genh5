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
GtH5::getObjectName(Location const& location) noexcept
{
    auto path = location.path().split('/');

    // should not be the case
    if (path.isEmpty())
    {
        qWarning() << "HDF5: Failed to retrieve the location name!";
        return {};
    }

    return path.last();
}

GtH5::Location::Location(std::shared_ptr<File> file) noexcept :
    m_file(std::move(file))
{ }

bool
GtH5::Location::isValid() const noexcept
{
    return Object::isValid() && m_file && m_file->isValid();
}

bool
GtH5::Location::exists(String const& path) const noexcept
{
    return exists(path.split('/').toVector());
}

bool
GtH5::Location::exists(Vector<String> const& path) const noexcept
{
    String subPath{};

    for (auto& entry : qAsConst(path))
    {
        subPath.push_back(entry);
        subPath.push_back('/');

//        if (H5Lexists(id(), subPath.constData(), H5P_DEFAULT))
        if (!toH5Location()->nameExists(subPath))
        {
            return false;
        }
    }

    return true;
}

GtH5::String
GtH5::Location::name() const noexcept
{
    return getObjectName(*this);
}

GtH5::ObjectType
GtH5::Location::type() const noexcept
{
    return H5Iget_type(id());
}

GtH5::Reference
GtH5::Location::toReference() noexcept(false)
{
    if (!isValid())
    {
        return {};
    }

    return GtH5::Reference(*this);
}

GtH5::String
GtH5::Location::path() const noexcept
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
