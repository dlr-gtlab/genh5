/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5dataspace.h"

#include <QDebug>

GtH5DataSpace::GtH5DataSpace(uint64_t simpleLength) :
    GtH5DataSpace(QVector<uint64_t>{ simpleLength })
{
}

GtH5DataSpace::GtH5DataSpace(std::initializer_list<uint64_t> dimensions):
    GtH5DataSpace(QVector<uint64_t>(dimensions))
{
}

GtH5DataSpace::GtH5DataSpace(const QVector<uint64_t>& dimensions)
{
    try
    {
        m_dataspace = H5::DataSpace(dimensions.length(),
                                    reinterpret_cast<const hsize_t*>
                                        (dimensions.constData()));
    }
    catch (H5::DataSpaceIException& /*e*/)
    {
        qCritical() << "HDF5: Creating dataspace failed!";
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5DataSpace:GtH5DataSpace failed!";
    }
}

GtH5DataSpace::GtH5DataSpace(const H5::DataSpace& dataspace) :
    m_dataspace(dataspace)
{
}

int64_t
GtH5DataSpace::id() const
{
    return m_dataspace.getId();
}

int
GtH5DataSpace::nDims() const
{
    return m_dataspace.getSimpleExtentNdims();
}

QVector<uint64_t>
GtH5DataSpace::dimensions() const
{
    QVector<uint64_t> dims(m_dataspace.getSimpleExtentNdims());
    m_dataspace.getSimpleExtentDims(reinterpret_cast<hsize_t*>(dims.data()));
    return dims;
}

H5::DataSpace
GtH5DataSpace::toH5() const
{
    return m_dataspace;
}

bool
GtH5DataSpace::operator==(const GtH5DataSpace& other)
{
    return this->dimensions() == other.dimensions();
}

bool
GtH5DataSpace::operator!=(const GtH5DataSpace& other)
{
    return !operator==(other);
}
