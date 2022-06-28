/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_OBJECT_H
#define GENH5_OBJECT_H

#include "genh5_exports.h"
#include "H5Cpp.h"


namespace GenH5
{

/**
 * @brief The Object class
 */
class GENH5_EXPORT Object
{
public:

    virtual ~Object() = default;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    virtual hid_t id() const noexcept = 0;

    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. This should be called after instanting any resource for
     * verification
     * @return whether object id is valid
     */
    virtual bool isValid() const noexcept;

    /**
     * @brief Static version of is valid
     * @param id id
     * @return is valid
     */
    static bool isValid(hid_t id) noexcept;

protected:

    Object(Object const& other) = default;
    Object(Object&& other) = default;
    Object& operator=(Object const& other) = default;
    Object& operator=(Object&& other) = default;

    /**
     * @brief Object
     */
    Object() = default;
};

} // namespace GenH5

#endif // GENH5_OBJECT_H
