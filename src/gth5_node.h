/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_NODE_H
#define GTH5_NODE_H

#include "gth5_location.h"


namespace GtH5
{
// forward decl
class DataType;
class DataSpace;
class Attribute;

/**
 * @brief The GtH5Leaf class
 */
class GTH5_EXPORT Node : public Location
{
public:

    /**
     * @brief returns whether the attribute exists at this node
     * @param name of the attribute
     * @return whether attibuet exists
     */
    bool hasAttribute(String const& name) const;

    GtH5::Attribute createAttribute(QString const& name,
                                    DataType const& dtype,
                                    DataSpace const& dspace) const;
    GtH5::Attribute createAttribute(const String &name,
                                    DataType const& dtype,
                                    DataSpace const& dspace) const;

    GtH5::Attribute openAttribute(QString const& name) const;
    GtH5::Attribute openAttribute(String const& name) const;

protected:

    /**
     * @brief GtH5Leaf
     */
    Node(std::shared_ptr<File> file = {});

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
};

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5Node = GtH5::Node;
#endif

#endif // GTH5_NODE_H
