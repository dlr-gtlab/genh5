/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5datatype.h"

#include <QDebug>

const GtH5DataType GtH5DataType::VarString = GtH5DataType(
            H5::StrType(H5::PredType::C_S1, H5T_VARIABLE));

GtH5DataType::GtH5DataType() = default;

GtH5DataType::GtH5DataType(const H5::DataType& type) :
    m_datatype{type}
{

}

GtH5DataType::GtH5DataType(GtH5DataType const& other) :
    m_datatype{other.m_datatype}
{
//    qDebug() << "GtH5DataType::copy";
}

GtH5DataType::GtH5DataType(GtH5DataType&& other) noexcept :
    m_datatype{std::move(other.m_datatype)}
{
//    qDebug() << "GtH5DataType::move";
}

GtH5DataType&
GtH5DataType::operator=(GtH5DataType const& other)
{
//    qDebug() << "GtH5DataType::copy=";
    auto tmp{other};
    swap(tmp);
    return *this;
}

GtH5DataType&
GtH5DataType::operator=(GtH5DataType&& other) noexcept
{
//    qDebug() << "GtH5DataType::move=";
    swap(other);
    return *this;
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

void
GtH5DataType::swap(GtH5DataType& other) noexcept
{
    using std::swap;
    swap(m_datatype, other.m_datatype);
}

bool
operator==(GtH5DataType const& first, GtH5DataType const& other)
{
    try
    {
        // check isValid to elimante "not a datatype" error from hdf5
        return first.id() == other.id() || (first.isValid() &&
                                            other.isValid() &&
                                            first.toH5() == other.toH5());
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
operator!=(GtH5DataType const& first, GtH5DataType const& other)
{
    return !(first == other);
}

void
swap(GtH5DataType& first, GtH5DataType& other) noexcept
{
//    qDebug() << "swap(GtH5DataType)";
    first.swap(other);
}
