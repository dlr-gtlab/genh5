/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_location.h"
#include "genh5_private.h"
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
    if (!location.isValid()) return  {};

    auto path = location.path();

    auto r_iter = std::find(std::rbegin(path), std::rend(path), '/');
    if (r_iter == std::rend(path))
    {
        log::ErrStream() << GENH5_MAKE_EXECEPTION_STR()
            "Failed to retrieve the location name!";
        return {};
    }

    bool isRoot = std::distance(std::rbegin(path), r_iter) < 1;

    // removes the leading '/'
    if (isRoot) r_iter++;

    return String{r_iter.base(), std::end(path)};
}

GenH5::String
GenH5::getObjectPath(GenH5::Location const& location) noexcept
{
    return GenH5::details::getName(location.id(),
                                   [](hid_t id, size_t len, char* data) {
         return H5Iget_name(id, data, len);
    });
}

GenH5::Location::Location() noexcept = default;

bool
GenH5::Location::isValid() const noexcept
{
    return Object::isValid();
}

bool
GenH5::Location::exists(hid_t locId, const char* name)
{
    return H5Lexists(locId, name, H5P_DEFAULT) != 0;
}

bool
GenH5::Location::exists(String const& path) const noexcept
{
    std::vector<std::string> elements;
    std::istringstream iss(path);
    std::string token;

    while (std::getline(iss, token, '/')) {
        elements.push_back(token);
    }

    return exists(elements);
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
