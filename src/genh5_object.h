/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_OBJECT_H
#define GENH5_OBJECT_H

#ifndef __GNUC__
#pragma warning( push )
#pragma warning(disable : 4251)
#endif

#include "H5Cpp.h"

#ifndef __GNUC__
#pragma warning( pop )
#endif

#include "genh5_exports.h"


namespace GenH5
{

using ObjectType = H5I_type_t;

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

    /**
     * @brief returns object type
     * @return object type
     */
    ObjectType type() const noexcept;

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
