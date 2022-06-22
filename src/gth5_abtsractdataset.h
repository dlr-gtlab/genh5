/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_ABTSRACTDATASET_H
#define GTH5_ABTSRACTDATASET_H

#include "gth5_exports.h"
#include "gth5_data.h"
#include "gth5_optional.h"

#include <QDebug>

namespace GtH5
{

/**
 * @brief The AbtsractDataSet class
 */
class GTH5_EXPORT AbstractDataSet
{
public:

    virtual ~AbstractDataSet() = default;

    /**
     * @brief dataType of this dataset
     * @return dataType
     */
    DataType dataType() const;
    /**
     * @brief dataSpace of this dataset
     * @return dataSpace
     */
    DataSpace dataSpace() const;

    /**
     * @brief writes data to dataset
     * @param data buffer to write
     * @return sucess
     */
    bool write(void const* data, Optional<DataType> dtype = {}) const;

    template<typename T>
    bool write(Vector<T> const& data, Optional<DataType> dtype = {}) const;
    template<typename T>
    bool write(AbstractData<T> const& data, Optional<DataType> dtype= {}) const;

    /**
     * @brief reads data from dataset
     * @param data buffer to write
     * @return sucess
     */
    bool read(void* data, Optional<DataType> dtype = {}) const;

    template<typename T>
    bool read(Vector<T>& data, Optional<DataType> dtype = {}) const;

    template<typename T>
    bool read(AbstractData<T>& data, Optional<DataType> dtype = {}) const;

protected:

    /**
     * @brief Returns the abstract hdf5 dataset
     * @return Abstract dset
     */
    virtual H5::AbstractDs const& toH5AbsDataSet() const = 0;

    /**
     * @brief Method for wirte implementation
     * @param data Data buffer to write
     * @return success
     */
    virtual bool doWrite(void const* data, DataType const& dtype) const = 0;

    /**
     * @brief Method for read implementation
     * @param data Data buffer to read
     * @return success
     */
    virtual bool doRead(void* data, DataType const& dtype) const = 0;

    /**
     * @brief AbtsractDataSet
     */
    AbstractDataSet();

    AbstractDataSet(AbstractDataSet const& other) = default;
    AbstractDataSet(AbstractDataSet&& other) = default;
    AbstractDataSet& operator=(AbstractDataSet const& other) = default;
    AbstractDataSet& operator=(AbstractDataSet&& other) = default;
};

template<typename T>
inline bool
AbstractDataSet::write(Vector<T> const& data, Optional<DataType> dtype) const
{
    if (data.size() < dataSpace().selectionSize())
    {
        auto dspace = dataSpace();
        qCritical() << "HDF5: Writing data failed!" <<
                       "(Too few data elements:"
                    << data.length() << "vs."
                    << dspace.dimensions()
                    << dspace.selectionSize() << "elements)";
        return false;
    }

    return write(data.data(), std::move(dtype));
}

template<typename T>
inline bool
AbstractDataSet::write(AbstractData<T> const& data,
                       Optional<DataType> dtype) const
{
    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return write(data.c(), std::move(dtype));
}

template<typename T>
inline bool
AbstractDataSet::read(Vector<T>& data, Optional<DataType> dtype) const
{
    auto dspace = dataSpace();
    data.resize(dspace.selectionSize());

    return read(data.data(), std::move(dtype));
}

template<typename T>
inline bool
AbstractDataSet::read(AbstractData<T>& data, Optional<DataType> dtype) const
{
    auto dspace = dataSpace();
    data.resize(dspace.selectionSize());

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return read(data.data(), std::move(dtype));
}

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5AbstractDataSet = GtH5::AbstractDataSet;
#endif

#endif // GTH5_ABTSRACTDATASET_H
