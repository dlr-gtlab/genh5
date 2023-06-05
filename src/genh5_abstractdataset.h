/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_ABSTRACTDATASET_H
#define GENH5_ABSTRACTDATASET_H

#include "genh5_exports.h"
#include "genh5_logging.h"
#include "genh5_data/base.h"
#include "genh5_optional.h"

namespace GenH5
{

/**
 * @brief The AbstractDataSet class
 */
class GENH5_EXPORT AbstractDataSet
{
public:

    virtual ~AbstractDataSet() = default;

    /**
     * @brief dataType of this dataset
     * @return dataType
     */
    virtual DataType dataType() const noexcept(false) = 0;
    /**
     * @brief dataSpace of this dataset
     * @return dataSpace
     */
    virtual DataSpace dataSpace() const noexcept(false) = 0;

    /**
     * @brief writes data to dataset
     * @param data buffer to write
     * @return sucess
     */
    bool write(void const* data,
               Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool write(Vector<T> const& data,
               Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool write(details::AbstractData<T> const& data,
               Optional<DataType> dtype = {}) const noexcept(false);

    /**
     * @brief reads data from dataset
     * @param data buffer to write
     * @return sucess
     */
    bool read(void* data,
              Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool read(Vector<T>& data,
              Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool read(details::AbstractData<T>& data,
              Optional<DataType> dtype = {}) const noexcept(false);

protected:

    /**
     * @brief Method for the write implementation
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
     * @brief AbstractDataSet
     */
    AbstractDataSet();

    AbstractDataSet(AbstractDataSet const& other) = default;
    AbstractDataSet(AbstractDataSet&& other) = default;
    AbstractDataSet& operator=(AbstractDataSet const& other) = default;
    AbstractDataSet& operator=(AbstractDataSet&& other) = default;
};

template<typename T>
inline bool
AbstractDataSet::write(Vector<T> const& data,
                       Optional<DataType> dtype) const noexcept(false)
{
    auto dspace = dataSpace();
    if (data.size() < dspace.selectionSize())
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Writing data failed! (too few data elements: "
                << data.size() << " vs. "
                << dspace.selectionSize() << " selected elements)";
        return false;
    }

    return write(data.data(), std::move(dtype));
}

template<typename T>
inline bool
AbstractDataSet::write(details::AbstractData<T> const& data,
                       Optional<DataType> dtype) const noexcept(false)
{
    auto dspace = dataSpace();
    if (static_cast<hssize_t>(data.size()) < dspace.selectionSize())
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Writing data failed! (too few data elements: "
                << data.size() << " vs. "
                << dspace.selectionSize() << " selected elements)";
        return false;
    }

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return write(data.dataWritePtr(), std::move(dtype));
}

template<typename T>
inline bool
AbstractDataSet::read(Vector<T>& data,
                      Optional<DataType> dtype) const noexcept(false)
{
    data.resize(dataSpace().selectionSize());
    return read(data.data(), std::move(dtype));
}

template<typename T>
inline bool
AbstractDataSet::read(details::AbstractData<T>& data,
                      Optional<DataType> dtype) const noexcept(false)
{
    auto dspace = dataSpace();
    if (!data.resize(dspace, dtype.isDefault() ? dataType() : *dtype))
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Reading data failed! (data container is too small: "
                << data.size() << " vs. "
                << dspace.selectionSize() << " selected elements)";
        return false;
    }

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return read(data.dataReadPtr(), std::move(dtype));
}

} // namespace GenH5

#endif // GENH5_ABSTRACTDATASET_H
