/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_LOCATION_H
#define GENH5_LOCATION_H

#include "genh5_object.h"
#include "genh5_typedefs.h"

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
    bool exists(String const& path) const noexcept;

    /**
     * @brief exists
     * @param path Path elements to check
     * @return exists
     */
    bool exists(Vector<String> const& path) const noexcept;

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
