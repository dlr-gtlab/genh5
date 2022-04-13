/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5OBJECT_H
#define GTH5OBJECT_H

#include "stdint.h"
#include "gth5_exports.h"
#include "H5Cpp.h"

/**
 * @brief The GtH5Object class
 */
class GTH5_EXPORT GtH5Object
{
public:

    virtual ~GtH5Object() = default;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    virtual int64_t id() const = 0;

    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether object id is valid
     */
    virtual bool isValid() const;

    static bool isValid(int64_t id);

protected:

    GtH5Object(GtH5Object const& other) = default;
    GtH5Object(GtH5Object&& other) = default;
    GtH5Object& operator=(GtH5Object const& other) = default;
    GtH5Object& operator=(GtH5Object&& other) = default;

    /**
     * @brief GtH5Object
     */
    GtH5Object();
};

#endif // GTH5OBJECT_H
