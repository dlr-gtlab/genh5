/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5datatype.h"

#include <QDebug>

const H5::StrType GtH5DataType::varStringType = H5::StrType(H5::PredType::C_S1,
                                                            H5T_VARIABLE);

GtH5DataType::GtH5DataType(const H5::DataType& type) :
    m_datatype(type)
{
}

int64_t
GtH5DataType::id() const
{
    return m_datatype.getId();
}

H5::DataType
GtH5DataType::toH5() const
{
    return m_datatype;
}

bool
GtH5DataType::operator==(const GtH5DataType& other)
{
    try
    {
        // check isValid to elimante "not a dtype" output from hdf5
        return this->id() == other.id() || (this->isValid() && other.isValid()&&
                                            this->toH5() == other.toH5());
    }
    catch (H5::DataTypeIException& /*e*/)
    {
        qWarning() << "HDF5: Datatype comparisson failed! (invalid data type)";
        return false;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataType:operator== failed!";
        return false;
    }
}

bool
GtH5DataType::operator!=(const GtH5DataType& other)
{
    return !operator==(other);
}

