/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_datasetcproperties.h"
#include "gth5_dataspace.h"

#include <QDebug>

static constexpr auto s_cmax = 9;
static constexpr auto s_cmin = 0;

GtH5::DataSetCProperties::DataSetCProperties() :
    m_properties(H5::DSetCreatPropList::DEFAULT)
{

}

GtH5::DataSetCProperties::DataSetCProperties(H5::DSetCreatPropList properties) :
    m_properties{std::move(properties)}
{

}

GtH5::DataSetCProperties::DataSetCProperties(Dimensions const& dimensions,
                                             int compression)
{
    setChunkDimensions(dimensions);
    setCompression(compression);
}

GtH5::Dimensions
GtH5::DataSetCProperties::autoChunk(DataSpace const& dataspace)
{
    Dimensions dimensions = dataspace.dimensions();

    std::replace_if(std::begin(dimensions), std::end(dimensions),
                    std::bind(std::less<>(),
                              std::placeholders::_1, 1),
                    1);

    // TODO: implement logic to generate optimal chunking layout

    return dimensions;
}

hid_t
GtH5::DataSetCProperties::id() const
{
    return m_properties.getId();
}

void
GtH5::DataSetCProperties::setChunkDimensions(Dimensions const& dimensions)
{
    if (dimensions.isEmpty())
    {
        qCritical() << "HDF5: Chunk dimensions are invalid";
        return;
    }
    m_properties.setChunk(dimensions.length(), dimensions.data());
}

void
GtH5::DataSetCProperties::setCompression(int level)
{
    if (level > s_cmax)
    {
        qWarning() << "HDF5: Compression level must be within 0 and 9! value:"
                   << level << "using:" << s_cmax;
        level = s_cmax;
    }
    else if (level < s_cmin)
    {
        qWarning() << "HDF5: Compression level must be within 0 and 9! value:"
                   << level << "using:" << s_cmin;
        level = s_cmin;
    }
    if (!isChunked() && level > s_cmin)
    {
        qCritical() << "HDF5: the dataset must be chunked first!";
        return;
    }

    m_properties.setDeflate(level);
}

bool
GtH5::DataSetCProperties::isChunked() const
{
    return H5Pget_layout(id()) == H5D_layout_t::H5D_CHUNKED;
}

//bool
//GtH5DataSetCProperties::isCompressed()
//{
//    qCritical() << "not implemented!";
//    return false;
//}

//int
//GtH5DataSetCProperties::compression()
//{
//    qCritical() << "not implemented!";
//    return -1;
//}

GtH5::Dimensions
GtH5::DataSetCProperties::chunkDimensions() const
{
    if (!isChunked())
    {
        return {};
    }

    Dimensions dims(m_properties.getChunk(0, nullptr));
    m_properties.getChunk(dims.length(), dims.data());
    return dims;
}

H5::DSetCreatPropList const&
GtH5::DataSetCProperties::toH5() const
{
    return m_properties;
}

template <typename Stream>
Stream
printDsetProps(Stream stream, GtH5::DataSetCProperties const& props)
{
    stream << "DsetProps{ ";
    stream << props.chunkDimensions();
    return stream << " }";
}

std::ostream&
operator<<(std::ostream& s, GtH5::DataSetCProperties const& d)
{
    return printDsetProps<std::ostream&>(s, d);
}

QDebug
operator<<(QDebug s, GtH5::DataSetCProperties const& d)
{
    QDebugStateSaver save{s};
    return printDsetProps<QDebug>(s.nospace(), d);
}
