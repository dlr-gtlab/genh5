/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_datasetcproperties.h"
#include "genh5_dataspace.h"
#include "genh5_private.h"

#include "H5Ppublic.h"

static constexpr int s_cmax = 9u;
static constexpr int s_cmin = 0u;

GenH5::DataSetCProperties::DataSetCProperties() :
    m_id(H5Pcreate(H5P_DATASET_CREATE))
{
    if (m_id < 0)
    {
        throw PropertyListException{
            GENH5_MAKE_EXECEPTION_STR() "Creating default properties failed"
        };
    }
}

GenH5::DataSetCProperties::DataSetCProperties(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

GenH5::DataSetCProperties::DataSetCProperties(Dimensions const& dimensions,
                                              int compression) :
    DataSetCProperties() // create default
{
    setChunkDimensions(dimensions);
    setDeflate(compression);
}

GenH5::DataSetCProperties GenH5::DataSetCProperties::fromId(hid_t id) noexcept
{
    DataSetCProperties d;
    d.m_id = id;
    return d;
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
    return m_id;
}

void
GenH5::DataSetCProperties::setChunkDimensions(Dimensions const& dimensions
                                             ) noexcept(false)
{
    if (dimensions.isEmpty())
    {
        throw PropertyListException{
            GENH5_MAKE_EXECEPTION_STR() "Chunking failed (Invalid dimensions)"
        };
    }

    if (H5Pset_chunk(m_id, dimensions.length(), dimensions.data()))
    {
        throw PropertyListException{
            GENH5_MAKE_EXECEPTION_STR() "Chunking failed"
        };
    }
}

void
GenH5::DataSetCProperties::setDeflate(int level) noexcept(false)
{
    if (level > s_cmax)
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Compression level must be within "
                << s_cmin
                << " and " << s_cmax << "! value: " << level
                << ", using: " << s_cmax;
        level = s_cmax;
    }
    else if (level < s_cmin)
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Compression level must be within "
                << s_cmin
                << " and " << s_cmax << "! value: " << level
                << ", using: " << s_cmin;
        level = s_cmin;
    }

    if (level == 0)
    {
        return; // nothing to do here
    }

    if (!isChunked() && level > s_cmin)
    {
        throw PropertyListException{
            GENH5_MAKE_EXECEPTION_STR()
            "Setting deflate compression failed (dataset must be chunked first)"
        };
    }

    if (H5Pset_deflate(m_id, static_cast<unsigned>(level)) < 0)
    {
        throw PropertyListException{
            GENH5_MAKE_EXECEPTION_STR() "Deflating failed"
        };
    }
}

bool
GenH5::DataSetCProperties::isChunked() const noexcept
{
    return H5Pget_layout(m_id) == H5D_layout_t::H5D_CHUNKED;
}

bool
GenH5::DataSetCProperties::isDeflated() const noexcept
{
    uint flags{};
    uint config{};

    return H5Pget_nfilters(m_id) &&
           H5Pget_filter_by_id(m_id, H5Z_FILTER_DEFLATE, &flags,
                               0, nullptr, 0, nullptr, &config) >= 0;
}

int
GenH5::DataSetCProperties::deflation() const noexcept
{
    uint flags{};       // 0 = mandatory, 1 = optional in pipeline
    size_t len{1};      // length of the filter data
    uint compression{}; // "compression level" is the first entry in the data
    uint config{};      // filter config - not relevant?

    herr_t err = H5Pget_filter_by_id(m_id, H5Z_FILTER_DEFLATE, &flags, &len,
                                     &compression, 0, nullptr, &config);

    return err < 0 ? 0 : compression;
}

GenH5::Dimensions
GenH5::DataSetCProperties::chunkDimensions() const noexcept
{
    if (!isChunked())
    {
        return {};
    }

    Dimensions dims(H5Pget_chunk(m_id, 0, nullptr));
    H5Pget_chunk(m_id, dims.size(), dims.data());
    return dims;
}

void
GenH5::DataSetCProperties::swap(DataSetCProperties& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
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
