/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_NODE_H
#define GENH5_NODE_H

#include "genh5_location.h"


namespace GenH5
{
// forward decl
class DataType;
class DataSpace;
class Attribute;
struct Version;

/**
 * @brief The GenH5Leaf class
 */
class GENH5_EXPORT Node : public Location
{
public:

    /**
     * @brief returns whether the attribute exists at this node
     * @param name of the attribute
     * @return whether attibuet exists
     */
    bool hasAttribute(String const& name) const;

    Attribute createAttribute(String const& name, DataType const& dtype,
                              DataSpace const& dspace) const noexcept(false);

    Attribute openAttribute(String const& name) const noexcept(false);

    static String versionAttrName();

    bool hasVersionAttribute() const;
    bool createVersionAttribute() const noexcept(false);
    Version readVersionAttribute() const noexcept(false);

protected:

    /**
     * @brief GenH5Leaf
     */
    Node(std::shared_ptr<File> file = {}) noexcept;

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    virtual H5::H5Object const* toH5Object() const noexcept = 0;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    H5::H5Location const* toH5Location() const noexcept override;
};

} // namespace GenH5

#endif // GENH5_NODE_H
