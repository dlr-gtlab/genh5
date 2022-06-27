/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_dataspace.h"

#include <QDebug>

GenH5::DataSpace
GenH5::DataSpace::Null{H5::DataSpace{H5S_NULL}};
GenH5::DataSpace
GenH5::DataSpace::Scalar{H5::DataSpace{H5S_SCALAR}};

GenH5::DataSpace::DataSpace() noexcept
    : m_dataspace{H5::DataSpace{H5S_NULL}}
{};

GenH5::DataSpace::DataSpace(Dimensions const& dimensions) noexcept(false)
{
    try
    {
        m_dataspace = H5::DataSpace(dimensions.size(),
                                    dimensions.constData());
    }
    catch (H5::DataSpaceIException const& e)
    {
        throw DataSpaceException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataSpace::DataSpace";
        throw DataSpaceException{e.getCDetailMsg()};
    }
}

GenH5::DataSpace::DataSpace(std::initializer_list<hsize_t> initlist
                           ) noexcept(false) :
    DataSpace{Dimensions{initlist}}
{ }

GenH5::DataSpace::DataSpace(H5::DataSpace dataspace):
    m_dataspace{std::move(dataspace)}
{ }

hid_t
GenH5::DataSpace::id() const noexcept
{
    return m_dataspace.getId();
}

int
GenH5::DataSpace::nDims() const noexcept
{
    return H5Sget_simple_extent_ndims(id());
}

bool
GenH5::DataSpace::isScalar() const noexcept
{
    return H5Sget_simple_extent_ndims(id()) == 0 && selectionSize() == 1;
}

bool
GenH5::DataSpace::isNull() const noexcept
{
    return H5Sget_simple_extent_ndims(id()) == 0 && selectionSize() == 0;
}

GenH5::Dimensions
GenH5::DataSpace::dimensions() const noexcept
{
    auto size = H5Sget_simple_extent_ndims(id());
    if (size < 0)
    {
        return {};
    }

    Dimensions dims;
    dims.resize(size);
    H5Sget_simple_extent_dims(id(), dims.data(), nullptr);
    return dims;
}

hssize_t
GenH5::DataSpace::selectionSize() const noexcept
{
    return H5Sget_select_npoints(id());
}

H5::DataSpace const&
GenH5::DataSpace::toH5() const noexcept
{
    return m_dataspace;
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
        throw DataSpaceException{"Selecting hyperslap failed "
                                 "(Invalid dataspace)"};
    }
    if (m_count.empty())
    {
        m_count = dims;
    }
    testSelection(m_count, dims, 1);
    testSelection(m_offset, dims, 0);
    testSelection(m_stride, dims, 1);
    testSelection(m_block, dims, 1);

    auto const& hspace = m_space.toH5();
    try
    {
        hspace.selectHyperslab(m_op, m_count.constData(), m_offset.constData(),
                               m_stride.constData(), m_block.constData());
    }
    catch (H5::DataSpaceIException const& e)
    {
        throw DataSpaceException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] DataSpaceSelection::commit";
        throw DataSpaceException{e.getCDetailMsg()};
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
        throw DataSpaceException{"Selecting hyperslap failed "
                                 "(Dimensions out of range)"};
    }
}
