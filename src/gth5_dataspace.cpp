/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_dataspace.h"

#include <QDebug>

GtH5::DataSpace
GtH5::DataSpace::Null{H5::DataSpace{H5S_NULL}};
GtH5::DataSpace
GtH5::DataSpace::Scalar{H5::DataSpace{H5S_SCALAR}};

GtH5::DataSpace::DataSpace()
    : m_dataspace{H5::DataSpace{H5S_NULL}}
{};

GtH5::DataSpace::DataSpace(Dimensions const& dimensions)
{
    try
    {
        m_dataspace = H5::DataSpace(dimensions.length(),
                                    dimensions.constData());
    }
    catch (H5::DataSpaceIException& /*e*/)
    {
        qCritical() << "HDF5: Creating dataspace failed!";
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::DataSpace:GtH5DataSpace failed!";
    }
}

GtH5::DataSpace::DataSpace(std::initializer_list<hsize_t> initlist) :
    DataSpace{Dimensions{initlist}}
{

}

GtH5::DataSpace::DataSpace(H5::DataSpace dataspace) :
    m_dataspace{std::move(dataspace)}
{

}

hid_t
GtH5::DataSpace::id() const
{
    return m_dataspace.getId();
}

int
GtH5::DataSpace::nDims() const
{
    return H5Sget_simple_extent_ndims(id());
}

bool
GtH5::DataSpace::isScalar() const
{
    return H5Sget_simple_extent_ndims(id()) == 0 && selectionSize() == 1;
}

bool
GtH5::DataSpace::isNull() const
{
    return H5Sget_simple_extent_ndims(id()) == 0 && selectionSize() == 0;
}

GtH5::Dimensions
GtH5::DataSpace::dimensions() const
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
GtH5::DataSpace::selectionSize() const
{
    return m_dataspace.getSelectNpoints();
}

H5::DataSpace const&
GtH5::DataSpace::toH5() const
{
    return m_dataspace;
}

bool
operator==(GtH5::DataSpace const& first, GtH5::DataSpace const& other)
{
    return first.dimensions() == other.dimensions();
}

bool
operator!=(GtH5::DataSpace const& first, GtH5::DataSpace const& other)
{
    return !(first == other);
}

template <typename Stream>
Stream
printDataSpace(Stream stream, GtH5::DataSpace const& space)
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

std::ostream&
operator<<(std::ostream& s, GtH5::DataSpace const& d)
{
    return printDataSpace<std::ostream&>(s, d);
}

QDebug
operator<<(QDebug s, GtH5::DataSpace const& d)
{
    QDebugStateSaver save{s};
    return printDataSpace<QDebug>(s.nospace(), d);
}

bool
GtH5::DataSpaceSelection::commit()
{
    auto dims = m_space.dimensions();
    if (!m_space.isValid() || dims.empty())
    {
        qCritical() << "HDF5: Selecting hyperslap failed! (Invalid dataspace)";
        return false;
    }
    if (m_count.empty())
    {
        m_count = dims;
        return true;
    }
    bool success = true;
    success &= testSelection(m_count, dims, 1);
    success &= testSelection(m_offset, dims, 0);
    success &= testSelection(m_stride, dims, 1);
    success &= testSelection(m_block, dims, 1);
    if (!success)
    {
        qCritical() << "HDF5: Selecting hyperslap failed! "
                       "(Dimensions out of range)";
        return false;
    }

    auto const& hspace = m_space.toH5();

    try
    {
        hspace.selectHyperslab(m_op,
                               m_count.constData(),
                               m_offset.constData(),
                               m_stride.constData(),
                               m_block.constData());
        return true;
    }
    catch (H5::DataSpaceIException& /*e*/)
    {
        qCritical() << "HDF5: Selecting hyperslap failed!";
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::makeHyperSlap failed!";
    }
    return false;
}

bool
GtH5::DataSpaceSelection::testSelection(Dimensions& dims,
                                        Dimensions const& sDims,
                                        hsize_t fillValue)
{
    auto ndims = sDims.size();
    auto size  = dims.size();
    // too few elements, fill with default value
    if (size < ndims)
    {
        dims.resize(ndims);
        std::fill(std::begin(dims) + size, std::end(dims), fillValue);
        return true;
    }
    // succeess if size == ndims
    return size == ndims;
}
