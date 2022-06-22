/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATASET_H
#define GTH5_DATASET_H

#include "gth5_node.h"
#include "gth5_abtsractdataset.h"
#include "gth5_datasetcproperties.h"

namespace GtH5
{
// forward decl
class File;
class Node;
class Reference;

/**
 * @brief The DataSet class
 */
class GTH5_EXPORT DataSet : public Node,
                            public AbstractDataSet
{
public:

    /**
     * @brief DataSet
     */
    DataSet();
    DataSet(std::shared_ptr<File> file, H5::DataSet dset);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataSet const& toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const override;

    /**
     * @brief deletes the object.
     * @return success
     */
    bool deleteLink() override;

    /**
     * @brief type of the object
     * @return type
     */
    ObjectType type() const override;

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    DataSetCProperties properties() const { return cProperties(); }

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    DataSetCProperties cProperties() const;

    /**
     * @brief resizes this dataset.
     * @param dimensions new dimensions. Cannot exceed the original dimensions.
     * Number of dimensions must match the current number
     * @return success
     */
    bool resize(Dimensions const& dimensions);

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

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
               Optional<DataType> dtype = {}) const;

    template<typename T>
    bool write(Vector<T> const& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace = {},
               Optional<DataType> dtype = {}) const;

    template<typename T>
    bool write(AbstractData<T> const& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace = {},
               Optional<DataType> dtype = {}) const;

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
              Optional<DataType> dtype = {});

    template<typename T>
    bool read(Vector<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype = {});

    template<typename T>
    bool read(AbstractData<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype = {});

protected:

    H5::AbstractDs const& toH5AbsDataSet() const override;

    bool doWrite(void const* data, DataType const& dtype) const override;
    bool doRead(void* data, DataType const& dtype) const override;

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    H5::H5Object const* toH5Object() const override;

private:

    /// hdf5 base instance
    H5::DataSet m_dataset{};

    friend class Reference;
};

template<typename T>
inline bool
DataSet::write(Vector<T> const& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace,
               Optional<DataType> dtype) const
{
    auto selected = fileSpace.selectionSize();

    if (selected > data.length())
    {
        qCritical() << "HDF5: Writing data failed!"
                    << "(Too few data elements for selection:"
                    << data.length() << "vs."
                    << selected << "selected)";
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
DataSet::write(AbstractData<T> const& data,
               DataSpace const& fileSpace,
               Optional<DataSpace> memSpace,
               Optional<DataType> dtype) const
{

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return write(static_cast<Vector<T>>(data),
                 fileSpace, std::move(memSpace), std::move(dtype));
}

template<typename T>
inline bool
DataSet::read(Vector<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype)
{
    data.resize(fileSpace.selectionSize());

    return read(data.data(), fileSpace, DataSpace::linear(data.size()),
                std::move(dtype));
}

template<typename T>
inline bool
DataSet::read(AbstractData<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype)
{
    data.resize(fileSpace.selectionSize());

    if (dtype.isDefault())
    {
        dtype = data.dataType();
    }

    return read(data.data(), fileSpace, data.dataSpace(), std::move(dtype));
}

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5DataSet = GtH5::DataSet;
#endif


#endif // GTH5_DATASET_H
