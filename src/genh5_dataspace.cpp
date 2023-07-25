/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_dataspace.h"
#include "genh5_private.h"

static const GenH5::DataSpace s_null = GenH5::DataSpace::fromId(H5Screate(H5S_NULL));
static const GenH5::DataSpace s_scalar = GenH5::DataSpace::fromId(H5Screate(H5S_SCALAR));

GenH5::DataSpace const& GenH5::DataSpace::Null = s_null;
GenH5::DataSpace const& GenH5::DataSpace::Scalar = s_scalar;

GenH5::DataSpace
GenH5::DataSpace::fromId(hid_t id)
{
    DataSpace d;
    d.m_id = id;
    return d;
}

GenH5::DataSpace::DataSpace() :
    DataSpace(H5Screate(H5S_NULL))
{ }

GenH5::DataSpace::DataSpace(H5S_class_t type) :
    m_id(H5Screate(type))
{
    if (m_id < 0)
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to create dataspace by type"
        };
    }
}

GenH5::DataSpace::DataSpace(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

GenH5::DataSpace::DataSpace(Dimensions const& dimensions) noexcept(false) :
    m_id(H5Screate_simple(numeric_cast<int>(dimensions.size()),
                          dimensions.data(),
                          nullptr))
{
    if (m_id < 0)
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to create simple dataspace"
        };
    }
}

GenH5::DataSpace::DataSpace(std::initializer_list<hsize_t> initlist
                            ) noexcept(false) :
    DataSpace{Dimensions{initlist}}
{ }

hid_t
GenH5::DataSpace::id() const noexcept
{
    return m_id;
}

int
GenH5::DataSpace::nDims() const noexcept
{
    return H5Sget_simple_extent_ndims(m_id);
}

bool
GenH5::DataSpace::isScalar() const noexcept
{
    return nDims() == 0 && selectionSize() == 1;
}

bool
GenH5::DataSpace::isNull() const noexcept
{
    return nDims() == 0 && selectionSize() == 0;
}

GenH5::Dimensions
GenH5::DataSpace::dimensions() const noexcept
{
    auto size = nDims();
    if (size < 0)
    {
        return {};
    }

    Dimensions dims;
    dims.resize(size);
    H5Sget_simple_extent_dims(m_id, dims.data(), nullptr);
    return dims;
}

hssize_t
GenH5::DataSpace::selectionSize() const noexcept
{
    return H5Sget_select_npoints(m_id);
}

void
GenH5::DataSpace::swap(DataSpace& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
}

bool
operator==(GenH5::DataSpace const& first, GenH5::DataSpace const& other)
{
    return first.dimensions() == other.dimensions();
}

bool
operator!=(GenH5::DataSpace const& first, GenH5::DataSpace const& other)
{
    return !(first == other);
}

#if 0
template <typename Stream>
Stream
printDataSpace(Stream stream, GenH5::DataSpace const& space)
{
    stream << "Space{ ";

    auto size = space.size();
    auto selection = space.selectionSize();

    if (space.isScalar())
    {
        return stream << "Scalar }";
    }
    if (space.isNull())
    {
        return stream << "Null }";
    }

    stream << space.dimensions();

    if (selection < size)
    {
        stream << " Sel: " << selection;
    }

    return stream << " }";
}
#endif

void
GenH5::DataSpaceSelection::commit() noexcept(false)
{
    auto dims = m_space.dimensions();
    if (!m_space.isValid() || dims.empty())
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR()
            "Selecting hyperslap failed (invalid dataspace)"
        };
    }
    if (m_count.empty())
    {
        m_count = dims;
    }
    testSelection(m_count, dims, 1); // TODO: use value of dataspace?
    testSelection(m_offset, dims, 0);
    testSelection(m_stride, dims, 1);
    testSelection(m_block, dims, 1);

    herr_t err = H5Sselect_hyperslab(m_space.id(), m_op,
                                     m_offset.data(), m_stride.data(),
                                     m_count.data(), m_block.data());

    if (err < 0)
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR() "Selecting hyperslap failed"
        };
    }
}

void
GenH5::DataSpaceSelection::testSelection(Dimensions& dim,
                                         Dimensions const& sDim,
                                         hsize_t fillValue) noexcept(false)
{
    auto ndims = sDim.size();
    auto size  = dim.size();
    // too few elements, fill with default value
    if (size < ndims)
    {
        dim.resize(ndims);
        std::fill(std::begin(dim) + size, std::end(dim), fillValue);
        size = dim.size();
    }
    // succeess if size == ndims
    if (size != ndims)
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR()
            "Selecting hyperslap failed (dimensions out of range)"
        };
    }
}
