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

#include <QList>

#include "H5Fpublic.h"
#include "H5Ppublic.h"

GenH5::String
GenH5::getObjectName(Location const& location) noexcept
{
    auto path = location.path().split('/');

    // should not be the case
    if (path.isEmpty())
    {
        log::ErrStream() << GENH5_MAKE_EXECEPTION_STR()
                            "Failed to retrieve the location name!";
        return {};
    }

    return path.last();
}

GenH5::String
GenH5::getObjectPath(GenH5::Location const& location) noexcept
{
    if (!location.isValid())
    {
        return {};
    }

    String buffer{32, ' '};
    auto bufferLen = static_cast<size_t>(buffer.size());

    auto acutalLen = static_cast<size_t>(H5Iget_name(location.id(),
                                                     buffer.data(),
                                                     bufferLen));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(static_cast<int>(bufferLen));
        H5Iget_name(location.id(), buffer.data(), bufferLen);
    }

    // chop of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}

GenH5::Location::Location() noexcept = default;

bool
GenH5::Location::isValid() const noexcept
{
    return Object::isValid();
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

        if (H5Lexists(id(), subPath.constData(), H5P_DEFAULT) == 0)
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

GenH5::Reference
GenH5::Location::toReference() const noexcept(false)
{
    return GenH5::Reference(*this);
}

GenH5::String
GenH5::Location::path() const noexcept
{
    return getObjectPath(*this);
}

GenH5::File GenH5::Location::file() const noexcept
{
    hid_t f = H5Iget_file_id(id());
    auto cleanup = finally(H5Fclose, f);
    Q_UNUSED(cleanup)
    return File(f);
}
