/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATASET_H
#define GENH5_DATASET_H

#include "genh5_node.h"
#include "genh5_abstractdataset.h"
#include "genh5_datasetcproperties.h"

namespace GenH5
{

/**
 * @brief The DataSet class
 */
class GENH5_EXPORT DataSet : public Node,
                             public AbstractDataSet
{
public:

    /**
     * @brief DataSet
     */
    DataSet();
    explicit DataSet(hid_t id);

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief deletes the object.
     */
    void deleteLink() noexcept(false) override;

    /*
     * @brief deletes the object tree recursively
     * (attributes)
     */
    void deleteRecursively() noexcept(false) override;

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    DataSetCProperties cProperties() const noexcept(false);

    /**
     * @brief dataType of this dataset
     * @return dataType
     */
    DataType dataType() const noexcept(false) override;
    /**
     * @brief dataSpace of this dataset
     * @return dataSpace
     */
    DataSpace dataSpace() const noexcept(false) override;

    /**
     * @brief resizes this dataset.
     * @param dimensions new dimensions. Cannot exceed the original dimensions.
     * Number of dimensions must match the current number
     * @return success
     */
    bool resize(Dimensions const& dimensions) noexcept(false) ;

    using AbstractDataSet::write;
    /**
     * @brief overload for writing selections
     * @param data buffer to write
     * @param fileSpace Dataspace selection for file layout
     * @param memSpace Dataspace selection for data layout
     * @return sucess
     */
    bool write(void const* data,
               DataSpace const& fileSpace,
               DataSpace const& memSpace,
               Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool write(Vector<T> const& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace = {},
               Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool write(details::AbstractData<T>& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace = {},
               Optional<DataType> dtype = {}) const noexcept(false);

    using AbstractDataSet::read;
    /**
     * @brief overload for reading selections
     * @param data buffer to read
     * @param fileSpace Dataspace selection for file layout
     * @param memSpace Dataspace selection for data layout
     * @return sucess
     */
    bool read(void* data,
              DataSpace const& fileSpace,
              DataSpace const& memSpace,
              Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool read(Vector<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype = {}) const noexcept(false);

    template<typename T>
    bool read(details::AbstractData<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype = {}) const noexcept(false);

    /// swaps all members
    void swap(DataSet& other) noexcept;

protected:

    /// write implementation
    bool doWrite(void const* data, DataType const& dtype) const override;
    /// read implementation
    bool doRead(void* data, DataType const& dtype) const override;

private:

    /// dataset id
    IdComponent<H5I_DATASET> m_id;

    friend class Reference;
};

template<typename T>
inline bool
DataSet::write(Vector<T> const& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace,
               Optional<DataType> dtype) const noexcept(false)
{
    auto selected = fileSpace.selectionSize();

    if (data.size() < selected)
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Writing data failed! (too few data elements: "
                << data.size() << " vs. "
                << selected << " selected elements)";
        return false;
    }

    if (memSpace.isDefault())
    {
        memSpace = DataSpace::linear(selected);
    }

    return write(data.constData(), fileSpace, memSpace, dtype);
}

template<typename T>
inline bool
DataSet::write(details::AbstractData<T>& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace,
               Optional<DataType> dtype) const noexcept(false)
{
    auto selected = fileSpace.selectionSize();

    if (data.size() < selected)
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Writing data failed! (too few data elements: "
                << data.size() << " vs. "
                << selected << " selected elements)";
        return false;
    }

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    if (memSpace.isDefault())
    {
        memSpace = DataSpace::linear(selected);
    }

    return write(data.dataWritePtr(), fileSpace, memSpace, std::move(dtype));
}

template<typename T>
inline bool
DataSet::read(Vector<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype) const noexcept(false)
{
    data.resize(fileSpace.selectionSize());

    return read(data.data(), fileSpace, DataSpace::linear(data.size()),
                std::move(dtype));
}

template<typename T>
inline bool
DataSet::read(details::AbstractData<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype) const noexcept(false)
{
    if (!data.resize(fileSpace, dtype.isDefault() ? dataType() : *dtype))
    {
        log::ErrStream()
                << GENH5_MAKE_EXECEPTION_STR()
                   "Reading data failed! (data container is too small: "
                << data.size() << " vs. "
                << fileSpace.selectionSize() << " selected elements)";
        return false;
    }

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return read(data.dataReadPtr(), fileSpace, data.dataSpace(),
                std::move(dtype));
}

} // namespace GenH5

inline void
swap(GenH5::DataSet& a, GenH5::DataSet& b) noexcept
{
    a.swap(b);
}


#endif // GENH5_DATASET_H
