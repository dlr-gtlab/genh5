/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 04.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5LEAF_H
#define GTH5LEAF_H

#include "gt_h5location.h"

class GtH5DataType;
class GtH5DataSpace;
class GtH5Attribute;

/**
 * @brief The GtH5Leaf class
 */
class GT_H5_EXPORT GtH5Leaf : public GtH5Location
{
public:

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    virtual int64_t id() const override = 0;

    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether object id is valid
     */
    virtual bool isValid() const override { return GtH5Location::isValid(); }

    /**
     * @brief deletes the object.
     * @return success
     */
    virtual bool deleteLink() override = 0;

    /**
     * @brief type of the object
     * @return type
     */
    virtual ObjectType type() const override = 0;

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    virtual const H5::H5Object* toH5Object() const = 0;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    virtual const H5::H5Location* toH5Location() const override;

    /**
     * @brief returns whether the attribute exists at this node
     * @param name of the attribute
     * @return whether attibuet exists
     */
    bool hasAttribute(const QByteArray& name) const;

    GtH5Attribute createAttribute(const QString& name,
                                  const GtH5DataType& dtype,
                                  const GtH5DataSpace& dspace) const;
    GtH5Attribute createAttribute(const QByteArray& name,
                                  const GtH5DataType& dtype,
                                  const GtH5DataSpace& dspace) const;

    GtH5Attribute openAttribute(const QString& name) const;
    GtH5Attribute openAttribute(const QByteArray& name) const;

protected:

    /**
     * @brief GtH5Leaf
     */
    GtH5Leaf() {}
};

#endif // GTH5LEAF_H
