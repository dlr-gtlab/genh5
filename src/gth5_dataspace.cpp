/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_dataspace.h"

#include <QDebug>


GtH5DataSpace::GtH5DataSpace() = default;

GtH5DataSpace::GtH5DataSpace(uint64_t simpleLength) :
    GtH5DataSpace(QVector<uint64_t>{ simpleLength })
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
    m_dataspace{dataspace}
{

}

GtH5DataSpace::GtH5DataSpace(GtH5DataSpace const& other) :
    m_dataspace{other.m_dataspace}
{
//    qDebug() << "GtH5DataSpace::copy";
}

GtH5DataSpace::GtH5DataSpace(GtH5DataSpace&& other) noexcept :
    m_dataspace{std::move(other.m_dataspace)}
{
//    qDebug() << "GtH5DataSpace::move";
}

GtH5DataSpace&
GtH5DataSpace::operator=(GtH5DataSpace const& other)
{
//    qDebug() << "GtH5DataSpace::copy=";
    auto tmp{other};
    swap(tmp);
    return *this;
}

GtH5DataSpace&
GtH5DataSpace::operator=(GtH5DataSpace&& other) noexcept
{
//    qDebug() << "GtH5DataSpace::move=";
    swap(other);
    return *this;
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
operator==(GtH5DataSpace const& first, GtH5DataSpace const& other)
{
    return first.dimensions() == other.dimensions();
}

bool
operator!=(GtH5DataSpace const& first, GtH5DataSpace const& other)
{
    return !(first == other);
}

void
GtH5DataSpace::swap(GtH5DataSpace& other) noexcept
{
    using std::swap;
    swap(m_dataspace, other.m_dataspace);
}

void
swap(GtH5DataSpace& first, GtH5DataSpace& other) noexcept
{
//    qDebug() << "swap(GtH5DataSpace)";
    first.swap(other);
}
