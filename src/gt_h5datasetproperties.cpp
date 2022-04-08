/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5datasetproperties.h"
#include "gt_h5dataspace.h"

#include <QDebug>


GtH5DataSetProperties::GtH5DataSetProperties() :
    m_properties(H5::DSetCreatPropList::DEFAULT)
{

}

GtH5DataSetProperties::GtH5DataSetProperties(
        H5::DSetCreatPropList const& properties) :
    m_properties(properties)
{

}

GtH5DataSetProperties::GtH5DataSetProperties(
        QVector<uint64_t> const& dimensions, int compression)
{
    setChunkDimensions(dimensions);
    setCompression(compression);
}

GtH5DataSetProperties::GtH5DataSetProperties(
        GtH5DataSetProperties const& other) :
    m_properties{other.m_properties}
{
//    qDebug() << "GtH5DataSpace::copy";
}

GtH5DataSetProperties::GtH5DataSetProperties(
        GtH5DataSetProperties&& other) noexcept :
    m_properties{std::move(other.m_properties)}
{
//    qDebug() << "GtH5DataSpace::move";
}

GtH5DataSetProperties&
GtH5DataSetProperties::operator=(GtH5DataSetProperties const& other)
{
//    qDebug() << "GtH5DataSpace::copy=";
    auto tmp{other};
    swap(tmp);
    return *this;
}

GtH5DataSetProperties&
GtH5DataSetProperties::operator=(GtH5DataSetProperties&& other) noexcept
{
//    qDebug() << "GtH5DataSpace::move=";
    swap(other);
    return *this;
}

QVector<uint64_t>
GtH5DataSetProperties::autoChunk(GtH5DataSpace const& dataspace, int maxChunks)
{
    QVector<uint64_t> dimensions(dataspace.dimensions());

    std::replace_if(std::begin(dimensions), std::end(dimensions),
                    std::bind(std::less<>(),
                              std::placeholders::_1, 1),
                    1);

    // TODO: implement logic to generate optimal chunking layout
    Q_UNUSED(maxChunks)

    return dimensions;
}

int64_t
GtH5DataSetProperties::id() const
{
    return m_properties.getId();
}

void
GtH5DataSetProperties::setChunkDimensions(QVector<uint64_t> const& dimensions)
{
    if (dimensions.isEmpty())
    {
        qCritical() << "HDF5: Chunk dimensions are invalid";
        return;
    }
    m_properties.setChunk(dimensions.length(),
                          reinterpret_cast<const hsize_t*>(dimensions.data()));
}

void
GtH5DataSetProperties::setCompression(int level)
{
    if (level > 9)
    {
        qWarning() << "HDF5: Compression level must be within 0 and 9! value:"
                   << level << "using: 9";
        level = 9;
    }
    else if (level < 0)
    {
        qWarning() << "HDF5: Compression level must be within 0 and 9! value:"
                   << level << "using: 0";
        level = 0;
    }
    if (!isChunked() && level > 0)
    {
        qCritical() << "HDF5: the dataset must be chunked first!";
        return;
    }

    m_properties.setDeflate(level);
}

bool
GtH5DataSetProperties::isChunked() const
{
    return H5Pget_layout(id()) == H5D_layout_t::H5D_CHUNKED;
}

//bool
//GtH5DataSetProperties::isCompressed()
//{
//    qCritical() << "not implemented!";
//    return false;
//}

//int
//GtH5DataSetProperties::compression()
//{
//    qCritical() << "not implemented!";
//    return -1;
//}

QVector<uint64_t>
GtH5DataSetProperties::chunkDimensions() const
{
    if (!isChunked())
    {
        return {};
    }

    QVector<uint64_t> dims(m_properties.getChunk(0, nullptr));
    m_properties.getChunk(dims.length(),
                          reinterpret_cast<hsize_t*>(dims.data()));
    return dims;
}

H5::DSetCreatPropList
GtH5DataSetProperties::toH5() const
{
    return m_properties;
}

void
GtH5DataSetProperties::swap(GtH5DataSetProperties& other) noexcept
{
    using std::swap;
    swap(m_properties, other.m_properties);
}

void
swap(GtH5DataSetProperties& first, GtH5DataSetProperties& other) noexcept
{
//    qDebug() << "swap(GtH5DataSetProperties)";
    first.swap(other);
}
