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
#include "genh5_utils.h"

#include <memory>


namespace GenH5
{
// forward decl
class File;
class Attribute;
class Reference;

using ObjectType = H5I_type_t;

/**
 * @brief The Location class
 */
class GENH5_EXPORT Location : public Object
{
public:

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
     * @brief returns object type
     * @return object type
     */
    ObjectType type() const noexcept;

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
     * @brief pointer to the shared file instance of this object
     * @return file
     */
    std::shared_ptr<File> const& file() const noexcept;

protected:

    /**
     * @brief Location
     */
    Location(std::shared_ptr<File> file = {}) noexcept;

    // shared file instance
    std::shared_ptr<File> m_file{};

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    virtual H5::H5Location const* toH5Location() const noexcept = 0;
};

/**
 * @brief helper function to retrieve the name of the location
 * @param location object
 * @return name
 */
GENH5_EXPORT String getObjectName(Location const& location) noexcept;

} // namespace GenH5

inline std::shared_ptr<GenH5::File> const&
GenH5::Location::file() const noexcept
{
    return m_file;
}

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
using GtH5Location = GenH5::Location;
#endif

#endif // GENH5_LOCATION_H
