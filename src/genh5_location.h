/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_LOCATION_H
#define GENH5_LOCATION_H

#include "genh5_stringview.h"
#include "genh5_object.h"
#include "genh5_utils.h"

#include <memory>

namespace GenH5
{
// forward decl
class File;
class Attribute;
class Reference;

/**
 * @brief The Location class
 */
class GENH5_EXPORT Location : public Object
{
public:

    using Object::isValid;
    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether object id is valid
     */
    bool isValid() const noexcept override;

    /**
     * @brief deletes the object.
     */
    virtual void deleteLink() noexcept(false) = 0;

    /**
     * @brief exists
     * @param path Path to check
     * @return exists
     */
    bool exists(StringView const& path) const noexcept;

    /**
     * @brief exists
     * @param path Path elements to check
     * @return exists
     */
    template <typename Container,
              typename T = traits::value_t<Container>,
              traits::if_convertible<T, String> = true>
    bool exists(Container const& path) const noexcept
    {
        hid_t id = this->id();
        String subPath{};

        for (auto const& entry : asConst(path))
        {
            subPath += entry;
            subPath += '/';

            if (!exists(id, subPath.data())) return false;
        }

        return true;
    }

    /**
     * @brief internal path
     * @return path
     */
    String path() const noexcept;

    /**
     * @brief object name
     * @return name
     */
    virtual String name() const noexcept;

    /**
     * @brief returns a reference to this object.
     * @return reference
     */
    Reference toReference() const noexcept(false);

    /**
     * @brief File object
     * @return file
     */
    File file() const noexcept;

protected:

    /**
     * @brief Location
     */
    explicit Location() noexcept;

private:

    /**
     * @brief Returns true if the name exists on the location denoted by locId.
     * @param locId Location id
     * @param name Name to querry
     * @return True if name exists in the location
     */
    static bool exists(hid_t locId, const char* name);
};

/**
 * @brief helper function to retrieve the name of the location
 * @param location object
 * @return name
 */
GENH5_EXPORT String getObjectName(Location const& location) noexcept;

/**
 * @brief helper function to retrieve the name of the location
 * @param location object
 * @return name
 */
GENH5_EXPORT String getObjectPath(Location const& location) noexcept;

} // namespace GenH5


#endif // GENH5_LOCATION_H
