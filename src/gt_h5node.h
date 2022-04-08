/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5location.h"

#ifndef GTH5NODE_H
#define GTH5NODE_H

class GtH5DataType;
class GtH5DataSpace;
class GtH5Attribute;

/**
 * @brief The GtH5Leaf class
 */
class GT_H5_EXPORT GtH5Node : public GtH5Location
{
public:

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    virtual H5::H5Object const* toH5Object() const = 0;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    H5::H5Location const* toH5Location() const override;

    /**
     * @brief returns whether the attribute exists at this node
     * @param name of the attribute
     * @return whether attibuet exists
     */
    bool hasAttribute(QByteArray const& name) const;

    GtH5Attribute createAttribute(QString const& name,
                                  GtH5DataType const& dtype,
                                  GtH5DataSpace const& dspace) const;
    GtH5Attribute createAttribute(QByteArray const& name,
                                  GtH5DataType const& dtype,
                                  GtH5DataSpace const& dspace) const;

    GtH5Attribute openAttribute(QString const& name) const;
    GtH5Attribute openAttribute(QByteArray const& name) const;

protected:

    /**
     * @brief GtH5Leaf
     */
    GtH5Node(std::shared_ptr<GtH5File> file = {},
                 QByteArray const& name = {});

    GtH5Node(GtH5Node const& other) = default;
    GtH5Node(GtH5Node&& other)  = default;
};

#endif // GTH5NODE_H
