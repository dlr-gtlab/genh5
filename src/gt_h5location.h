/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5LOCATION_H
#define GTH5LOCATION_H

#include "gt_h5object.h"

#include <QByteArray>
#include <QByteArrayList>

class GtH5File;
class GtH5Attribute;
class GtH5Reference;

/**
 * @brief The GtH5Location class
 */
class GT_H5_EXPORT GtH5Location : public GtH5Object
{
public:

    /**
     * @brief The ObjectType enum
     */
    enum ObjectType
    {
        Unkown,     // invalid object type
        Group,      // group
        DataSet,    // dataset
        Attribute   // attribute
    };

    /**
     * @brief id or handle of the hdf5 resource.
     * @return id
     */
    virtual int64_t id() const override = 0;

    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether object id is valid
     */
    virtual bool isValid() const override;

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
    virtual const H5::H5Location* toH5Location() const = 0;

    /**
     * @brief exists
     * @param path
     * @return
     */
    bool exists(const QByteArray& path) const;

    /**
     * @brief exists
     * @param path
     * @return
     */
    bool exists(const QByteArrayList& path) const;

    /**
     * @brief internal path
     * @return path
     */
    QByteArray path() const;

    /**
     * @brief internal name
     * @return name
     */
    QByteArray name() const;

    /**
     * @brief returns a reference to this object.
     * @return reference
     */
    GtH5Reference toReference();

    /**
     * @brief pointer to the file instance of this object. File instance must
     * be kept alive for the duration of any operation.
     * @return file
     */
    GtH5File* file() const { return m_file; }

protected:

    /**
     * @brief GtH5Location
     */
    GtH5Location() : m_file(Q_NULLPTR), m_name(QByteArrayLiteral("")) {}

    /// file instance of this object. Must be kept alive for the duration of any operation.
    GtH5File* m_file;
    /// name of this location
    QByteArray m_name;

    /**
     * @brief helper function to retrieve the name of the location
     * @param location
     * @return name
     */
    static QByteArray getObjectName(GtH5Location& location);

    /**
     * @brief helper function to retrieve the name of an attribute
     * @param attr
     * @return name
     */
    static QByteArray getAttrName(GtH5Attribute& attr);
};

#endif // GTH5LOCATION_H
