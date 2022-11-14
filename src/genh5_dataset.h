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
    explicit DataSet();
    explicit DataSet(std::shared_ptr<File> file, H5::DataSet dset);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataSet const& toH5() const noexcept;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief deletes the object.
     */
    void deleteLink() noexcept(false) override;

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    DataSetCProperties properties() const noexcept(false)
    {
        return cProperties();
    }

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    DataSetCProperties cProperties() const noexcept(false);

    /**
     * @brief resizes this dataset.
     * @param dimensions new dimensions. Cannot exceed the original dimensions.
     * Number of dimensions must match the current number
     * @return success
     */
    bool resize(Dimensions const& dimensions) noexcept(false) ;

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
              Optional<DataType> dtype = {}) noexcept(false);

    template<typename T>
    bool read(Vector<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype = {}) noexcept(false);

    template<typename T>
    bool read(details::AbstractData<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype = {}) noexcept(false);

    /*
     *  WRITE ATTRIBUTE
     */

    /**
     * @brief Delegates the function call to Node::writeAttribute
     * @param name Name of attribute
     * @param data Data to write
     * @return This
     */
    template <typename T>
    DataSet const& writeAttribute(String const& name, T&& data
                                  ) const noexcept(false)
    {
        Node::writeAttribute(name, std::forward<T>(data));
        return *this;
    }

    /**
     * @brief Delegates the function call to Node::writeAttribute0D
     * @param name Name of attribute
     * @param data 0D Data to write
     * @return This
     */
    template <typename Container,
              traits::if_has_not_template_type<Container> = true>
    DataSet const& writeAttribute0D(String const& name, Container&& data
                                    ) const noexcept(false)
    {
        Node::writeAttribute0D(name, std::forward<Container>(data));
        return *this;
    }

    /**
     * @brief Delegates the function call to Node::writeVersionAttribute
     * @param string Attribute name
     * @param version Version to write
     * @return This
     */
    DataSet const& writeVersionAttribute(String const& string = versionAttributeName(),
                                         Version version = Version::current()
                                         ) const noexcept(false)
    {
        Node::writeVersionAttribute(string, version);
        return *this;
    }

protected:

    /// Returns the abstract hdf5 dataset
    H5::AbstractDs const& toH5AbsDataSet() const noexcept override;

    /// write implementation
    bool doWrite(void const* data, DataType const& dtype) const override;
    /// read implementation
    bool doRead(void* data, DataType const& dtype) const override;

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    H5::H5Object const* toH5Object() const noexcept override;

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
               Optional<DataType> dtype) const noexcept(false)
{
    auto selected = fileSpace.selectionSize();

    if (data.size() < selected)
    {
        debugWriteError(data.size());
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
        debugWriteError(data.size(), fileSpace);
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
              Optional<DataType> dtype) noexcept(false)
{
    data.resize(fileSpace.selectionSize());

    return read(data.data(), fileSpace, DataSpace::linear(data.size()),
                std::move(dtype));
}

template<typename T>
inline bool
DataSet::read(details::AbstractData<T>& data,
              DataSpace const& fileSpace,
              Optional<DataType> dtype) noexcept(false)
{
    if (!data.resize(fileSpace, dtype.isDefault() ? dataType() : *dtype))
    {
        debugReadError(data.size(), fileSpace);
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


#endif // GENH5_DATASET_H
