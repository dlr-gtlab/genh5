/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5LOCATION_H
#define GTH5LOCATION_H

#include "gth5_object.h"

#include <QByteArray>
#include <QByteArrayList>
#include <memory>

class GtH5File;
class GtH5Attribute;
class GtH5Reference;

/**
 * @brief The GtH5Location class
 */
class GTH5_EXPORT GtH5Location : public GtH5Object
{
public:

    /**
     * @brief The ObjectType enum
     */
    enum ObjectType
    {
        Unkown = 1,     // invalid object type
        Group = 2,      // group
        DataSet = 4,    // dataset
        Attribute = 8   // attribute
    };

    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether object id is valid
     */
    bool isValid() const override;

    /**
     * @brief deletes the object.
     * @return success
     */
    virtual bool deleteLink() = 0;

    /**
     * @brief type of the object
     * @return type
     */
    virtual ObjectType type() const;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    virtual H5::H5Location const* toH5Location() const = 0;

    /**
     * @brief exists
     * @param path
     * @return
     */
    bool exists(QByteArray const& path) const;

    /**
     * @brief exists
     * @param path
     * @return
     */
    bool exists(QByteArrayList const& path) const;

    /**
     * @brief internal path
     * @return path
     */
    QByteArray path() const;

    /**
     * @brief internal name
     * @return name
     */
    QByteArray const& name() const;

    /**
     * @brief returns a reference to this object.
     * @return reference
     */
    GtH5Reference toReference();

    /**
     * @brief pointer to the shared file instance of this object
     * @return file
     */
    std::shared_ptr<GtH5File> file() const;

protected:

    /**
     * @brief GtH5Location
     */
    GtH5Location(std::shared_ptr<GtH5File> file = {},
                 QByteArray const& name = {});

    // shared file instance
    std::shared_ptr<GtH5File> m_file{};
    /// name of this location
    QByteArray m_name{};

    /**
     * @brief helper function to retrieve the name of the location
     * @param location
     * @return name
     */
    static QByteArray getObjectName(GtH5Location const& location);

    /**
     * @brief helper function to retrieve the name of an attribute
     * @param attr
     * @return name
     */
    static QByteArray getAttrName(GtH5Attribute const& attr);
};

#endif // GTH5LOCATION_H
