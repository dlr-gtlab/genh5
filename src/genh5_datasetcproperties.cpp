/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_datasetcproperties.h"
#include "genh5_dataspace.h"

#include <QDebug>

static constexpr auto s_cmax = 9;
static constexpr auto s_cmin = 0;

GenH5::DataSetCProperties::DataSetCProperties() noexcept:
    m_properties(H5::DSetCreatPropList::DEFAULT)
{ }

GenH5::DataSetCProperties::DataSetCProperties(H5::DSetCreatPropList properties) :
    m_properties{std::move(properties)}
{ }

GenH5::DataSetCProperties::DataSetCProperties(Dimensions const& dimensions,
                                             int compression)
{
    setChunkDimensions(dimensions);
    setCompression(compression);
}

GenH5::Dimensions
GenH5::DataSetCProperties::autoChunk(DataSpace const& dataspace) noexcept
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
GenH5::DataSetCProperties::id() const noexcept
{
    return m_properties.getId();
}

void
GenH5::DataSetCProperties::setChunkDimensions(Dimensions const& dimensions
                                             ) noexcept(false)
{
    if (dimensions.isEmpty())
    {
        throw DataSetException{"Chunking failed (Invalid dimensions)"};
    }

    try
    {
        m_properties.setChunk(dimensions.length(), dimensions.data());
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataSetCProperties::"
                       "setChunkDimensions";
        throw DataSetException{e.getCDetailMsg()};
    }
}

void
GenH5::DataSetCProperties::setCompression(int level) noexcept(false)
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
        throw DataSetException{"Setting compression failed (Dataset must be "
                               "chunked first)"};
    }

    try
    {
        m_properties.setDeflate(level);
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataSetCProperties::setCompression";
        throw DataSetException{e.getCDetailMsg()};
    }
}

bool
GenH5::DataSetCProperties::isChunked() const noexcept
{
    return H5Pget_layout(id()) == H5D_layout_t::H5D_CHUNKED;
}

//bool
//GenH5DataSetCProperties::isCompressed()
//{
//    qCritical() << "not implemented!";
//    return false;
//}

//int
//GenH5DataSetCProperties::compression()
//{
//    qCritical() << "not implemented!";
//    return -1;
//}

GenH5::Dimensions
GenH5::DataSetCProperties::chunkDimensions() const noexcept
{
    if (!isChunked())
    {
        return {};
    }

    Dimensions dims(H5Pget_chunk(id(), 0, nullptr));
    H5Pget_chunk(id(), dims.size(), dims.data());
    return dims;
}

H5::DSetCreatPropList const&
GenH5::DataSetCProperties::toH5() const noexcept
{
    return m_properties;
}

#if 0
template <typename Stream>
Stream
printDsetProps(Stream stream, GenH5::DataSetCProperties const& props)
{
    stream << "DsetProps{ ";
    stream << props.chunkDimensions();
    return stream << " }";
}
#endif
