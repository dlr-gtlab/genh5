/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5REFERENCE_H
#define GTH5REFERENCE_H

#include "gth5_location.h"

class GtH5File;
class GtH5Group;
class GtH5DataSet;
class GtH5Attribute;

/**
 * @brief The GtH5Reference class
 */
class GTH5_EXPORT GtH5Reference
{

public:

    /// typedef for ObjectType enum
    using ObjectType = GtH5Location::ObjectType;

    GtH5Reference();
    explicit GtH5Reference(int64_t data,
                           ObjectType type = ObjectType::Unkown);
    explicit GtH5Reference(H5R_ref_t const& ref,
                           ObjectType type = ObjectType::Unkown);
    explicit GtH5Reference(GtH5Location const& location);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5R_ref_t toH5() const;

    /**
     * @brief returns whether the reference is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether reference is valid
     */
    bool isValid() const;

    /**
     * @brief returns the internal reference
     * @return internal reference
     */
    int64_t data() const;

    /**
     * @brief type of the referenced object (may be Unkown)
     * @return type
     */
    ObjectType type() const;

    /**
     * @brief try dereferencing the object to a group.
     * @param file in which the object is located
     * @return group
     */
    GtH5Group toGroup(GtH5File const& file) const;

    /**
     * @brief try dereferencing the object to a dataset.
     * @param file in which the object is located
     * @return dataset
     */
    GtH5DataSet toDataSet(GtH5File const& file) const;

    /**
     * @brief try dereferencing the object to a attribute.
     * @param file in which the object is located
     * @return attribute
     */
    GtH5Attribute toAttribute(GtH5File const& file) const;

private:

    /// hdf5 base instance
    H5R_ref_t m_ref{};
    /// type of the referenced object
    ObjectType m_type{ObjectType::Unkown};
};

#endif // GTH5REFERENCE_H
