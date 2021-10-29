/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATATYPE_H
#define GTH5DATATYPE_H

#include "gt_h5object.h"
#include <QVector>

/**
 * @brief The GtH5DataType class
 */
class GT_H5_EXPORT GtH5DataType : public GtH5Object
{
public:

    static const H5::StrType varStringType;

    GtH5DataType() {}
    GtH5DataType(const H5::DataType& type);

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

    // operators
    bool operator==(const GtH5DataType& other);
    bool operator!=(const GtH5DataType& other);

protected:

    /// hdf5 base instance
    H5::DataType m_datatype;
};


#endif // GTH5DATATYPE_H
