/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_dataset.h"
#include "genh5_private.h"
#include "genh5_reference.h"
#include "genh5_node.h"
#include "genh5_file.h"

#include "H5Dpublic.h"
#include "H5Ppublic.h"

GenH5::DataSet::DataSet() = default;

GenH5::DataSet::DataSet(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

void
GenH5::DataSet::swap(DataSet& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
}

hid_t
GenH5::DataSet::id() const noexcept
{
    return m_id;
}

GenH5::DataSetCProperties
GenH5::DataSet::cProperties() const noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR()
            "Failed to access dataset creation properties";

    if (!isValid())
    {
        throw DataSetException{
            GENH5_MAKE_EXECEPTION_STR()
            "Failed to open cProperties (invalid dataset id)"
        };
    }

    return details::make<DataSetCProperties, DataSetException>([id = m_id](){
        return H5Dget_create_plist(id);
    }, errMsg);
}

bool
GenH5::DataSet::doWrite(void const* data, DataType const& dtype) const
{
    auto dspace = dataSpace();

    herr_t err = H5Dwrite(m_id, dtype.id(), dspace.id(), dspace.id(),
                          H5P_DEFAULT, data);

    return err >= 0;
}

bool
GenH5::DataSet::doRead(void* data, DataType const& dtype) const
{
    auto dspace = dataSpace();

    herr_t err = H5Dread(m_id, dtype.id(), dspace.id(), dspace.id(),
                         H5P_DEFAULT, data);

    return err >= 0;
}

bool
GenH5::DataSet::write(void const* data,
                      DataSpace const& fileSpace,
                      DataSpace const& memSpace,
                      Optional<DataType> dtype) const
{
    auto space = dataSpace();
    if (space.isNull())
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR() "Writing dataset '"
                << path() << "' failled (null dataspace)";
        return false;
    }
    if (!data)
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR() "Writing dataset '"
                << path() << "' failled (invalid data)";
        return false;
    }

    auto type = dataType();
    if (dtype.isDefault() || dtype == type)
    {
        dtype = type;
    }

    herr_t err = H5Dwrite(m_id, dtype->id(), memSpace.id(), fileSpace.id(),
                          H5P_DEFAULT, data);

    return err >= 0;
}

bool
GenH5::DataSet::read(void* data,
                     DataSpace const& fileSpace,
                     DataSpace const& memSpace,
                     Optional<DataType> dtype) const
{
    auto space = dataSpace();
    if (space.isNull())
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR() "Reading dataset '"
                << path() << "' failled (null dataspace)";
        return false;
    }
    if (!data)
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR() "Reading dataset '"
                << path() << "' failled (invalid data)";
        return false;
    }

    auto type = dataType();
    if (dtype.isDefault() || dtype == type)
    {
        dtype = type;
    }

    herr_t err = H5Dread(m_id, dtype->id(), memSpace.id(), fileSpace.id(),
                         H5P_DEFAULT, data);

    return err >= 0;
}

void
GenH5::DataSet::deleteLink() noexcept(false)
{
    if (!isValid())
    {
        throw LocationException{
            GENH5_MAKE_EXECEPTION_STR() "Deleting dataset failed (invalid id)"
        };
    }

    auto m_dataspace = dataSpace();
    auto dims = m_dataspace.dimensions();
    dims.fill(0);

    // resize dataset to 0
    if (cProperties().isChunked())
    {
        resize(dims);
    }

    // returns error type
    if (H5Ldelete(file().id(), path().constData(), H5P_DEFAULT) < 0)
    {
        throw LocationException{
            GENH5_MAKE_EXECEPTION_STR() "Deleting dataset '" +
            path().toStdString() + "' failed"
        };
    }
    close();
}

void
GenH5::DataSet::deleteRecursively() noexcept(false)
{
    // attributes
    for (auto& attr : findAttributes())
    {
        attr.toAttribute(*this).deleteLink();
    }

    // this
    deleteLink();
}

GenH5::DataType
GenH5::DataSet::dataType() const noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to access datatype";

    if (!isValid())
    {
        throw DataTypeException{
            GENH5_MAKE_EXECEPTION_STR() "Accessing datatype failed (invalid id)"
        };
    }

    return details::make<DataType, DataTypeException>([id = m_id](){
        return H5Dget_type(id);
    }, errMsg);
}

GenH5::DataSpace
GenH5::DataSet::dataSpace() const noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to access dataspace";

    if (!isValid())
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR() "Accessing dataspace failed (invalid id)"
        };
    }

    return details::make<DataSpace, DataSpaceException>([id = m_id](){
        return H5Dget_space(id);
    }, errMsg);
}

bool
GenH5::DataSet::resize(Dimensions const& dimensions) noexcept(false)
{
    // dataset must be chunked
    if (!cProperties().isChunked())
    {
        log::ErrStream() << GENH5_MAKE_EXECEPTION_STR()
                            "Resizing dataset failed! (not chunked)";
        return false;
    }

    // nDims must be equal
    auto dspace = dataSpace();
    if (dimensions.length() != dspace.nDims())
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Resizing dataset failed! (n-dims mismatch: "
                << dimensions.length() << " vs. " << dspace.nDims() << ")";
        return false;
    }

    // check if resizing is necessary
    herr_t err = 0;
    if (dimensions != dspace.dimensions())
    {
        err = H5Dset_extent(m_id, dimensions.constData());
    }
    return err >= 0;
}

void
GenH5::DataSet::close()
{
    m_id.dec();
}
