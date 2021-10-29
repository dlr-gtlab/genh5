/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5NODE_H
#define GTH5NODE_H

#include "gt_h5leaf.h"

class GtH5DataType;
class GtH5DataSpace;
class GtH5DataSet;
class GtH5Group;

/**
 * @brief The GtH5Node class
 */
class GT_H5_EXPORT GtH5Node : public GtH5Leaf
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
    virtual bool isValid() const override { return GtH5Leaf::isValid(); }

    /**
     * @brief deletes the object
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
    virtual const H5::H5Object* toH5Object() const override = 0;

    // groups
    GtH5Group createGroup(const QString& name);
    GtH5Group createGroup(const QByteArray& name);
    GtH5Group openGroup(const QString& name);
    GtH5Group openGroup(const QByteArray& name);

    // datasets
    GtH5DataSet createDataset(const QString& name,
                              const GtH5DataType& dtype,
                              const GtH5DataSpace& dspace);
    GtH5DataSet createDataset(const QByteArray& name,
                              const GtH5DataType& dtype,
                              const GtH5DataSpace& dspace);
    GtH5DataSet openDataset(const QString& name);
    GtH5DataSet openDataset(const QByteArray& name);

protected:

    GtH5Node() {}
};

#endif // GTH5NODE_H
