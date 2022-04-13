/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATATYPE_H
#define GTH5DATATYPE_H

#include "gth5_object.h"
#include <QVector>

/**
 * @brief The GtH5DataType class
 */
class GTH5_EXPORT GtH5DataType : public GtH5Object
{
public:

    static const GtH5DataType VarString;

    GtH5DataType();
    explicit GtH5DataType(H5::DataType const& type);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataType toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    int64_t id() const override;

protected:

    /// hdf5 base instance
    H5::DataType m_datatype{};
};

// operators
GTH5_EXPORT bool operator==(GtH5DataType const& first,
                             GtH5DataType const& other);
GTH5_EXPORT bool operator!=(GtH5DataType const& first,
                             GtH5DataType const& other);

#endif // GTH5DATATYPE_H
