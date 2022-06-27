/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_location.h"
#include "genh5_reference.h"
#include "genh5_group.h"
#include "genh5_file.h"
#include "genh5_attribute.h"

#include <QDebug>

GenH5::String
GenH5::getObjectName(Location const& location) noexcept
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

GenH5::Location::Location(std::shared_ptr<File> file) noexcept :
    m_file(std::move(file))
{ }

bool
GenH5::Location::isValid() const noexcept
{
    return Object::isValid() && m_file && m_file->isValid();
}

bool
GenH5::Location::exists(String const& path) const noexcept
{
    return exists(path.split('/').toVector());
}

bool
GenH5::Location::exists(Vector<String> const& path) const noexcept
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

GenH5::String
GenH5::Location::name() const noexcept
{
    return getObjectName(*this);
}

GenH5::ObjectType
GenH5::Location::type() const noexcept
{
    return H5Iget_type(id());
}

GenH5::Reference
GenH5::Location::toReference() const noexcept(false)
{
    return GenH5::Reference(*this);
}

GenH5::String
GenH5::Location::path() const noexcept
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
