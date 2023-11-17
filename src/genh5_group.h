/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_GROUP_H
#define GENH5_GROUP_H

#include "genh5_node.h"

#include "genh5_datasetcproperties.h"
#include "genh5_dataset.h"
#include "genh5_optional.h"

namespace GenH5
{

/**
 * @brief The IterationType enum. use to specify the search depth when
 * iterating over child nodes.
 */
enum IterationType
{
    FindDirectOnly = 0,
    FindRecursive = 1
};

/**
 * @brief The IterationFilter enum. Can be used to filter out certain object
 * type when iterating over child nodes.
 */
enum IterationFilter
{
    NoFilter = 0,
    FilterGroups = 1,
    FilterDataSets = 2
};

/// callback function type for iterating over nodes
using NodeIterationFunction =
    std::function<herr_t(Group const&, NodeInfo const&)>;

/**
 * @brief The Group class
 */
class GENH5_EXPORT Group : public Node
{
public:

    /**
     * @brief Group
     */
    Group();
    explicit Group(File const& file);
    explicit Group(hid_t id);

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
     * (subgroups, datasets and attributes)
     */
    void deleteRecursively() noexcept(false) override;

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

    /*
     * CREATE/OPEN GROUP
     */

    /**
     * @brief Creates a group. If the group already exists,it will be opened.
     * @param name name of the group
     * @return Group
     */
    Group createGroup(String const& name) const noexcept(false);

    /**
     * @brief Creates a group. If the group already exists,it will be opened.
     * @param name name of the group
     * @return Group
     */
    Group openGroup(String const& name) const noexcept(false);

    /*
     * CREATE/OPEN DATASET
     */

    /**
     * @brief Creates a dataset. The dataset has no values set. If the
     * dataset already exists, its datatype and dataspace are checked. If they
     * match the dataset will be opened else it will be overwritten.
     * @param name Name of the dataset
     * @param dtype Datatype of the dataset
     * @param dspace Dataspace of the dataset
     * @param cProps Optional Create properties. By default dataset will be
     * chunked but not compressed.
     * @return Dataset
     */
    DataSet createDataSet(String const& name,
                          DataType const& dtype,
                          DataSpace const& dspace,
                          Optional<DataSetCProperties> cProps = {}
                          ) const noexcept(false);

    /**
     * @brief Opens the dataset specified.
     * @param name Name of the dataset
     * @return Dataset
     */
    DataSet openDataSet(String const& name) const noexcept(false);

    /*
     *  READ/WRITE DATASET
     */

    /**
     * @brief High level method for creating and writing to a dataset.
     * @param name Name of the dataset
     * @return data Data to write
     * @return Dataset
     */
    template <typename Container,
              typename if_container = traits::value_t<Container>,
              traits::if_has_not_template_type<Container> = true>
    DataSet writeDataSet(String const& name, Container&& data
                         ) const noexcept(false);

    /**
     * @brief Overload.
     * @param name Name of the dataset
     * @return data Data to write
     * @return Dataset
     */
    template <typename T>
    DataSet writeDataSet(String const& name,
                         details::AbstractData<T> const& data
                         ) const noexcept(false);

    /**
     * @brief High level method for creating and writing to a dataset.
     * Cannot be used to write an object of type Data0D.
     * @param name Name of the dataset
     * @return data 0D Data to write
     * @return Dataset
     */
    template <typename Container,
              traits::if_has_not_template_type<Container> = true>
    DataSet writeDataSet0D(String const& name, Container&& data
                           ) const noexcept(false);

    /**
     * @brief High level method for opening and reading data from a dataset.
     * @param name Name of the dataset
     * @return Data read
     */
    template <typename T1, typename... Ts>
    Data<T1, Ts...> readDataSet(String const& name) const noexcept(false);

    /**
     * @brief High level method for opening and reading 0d data from a dataset.
     * @param name Name of the dataset
     * @return Data read
     */
    template <typename T1, typename... Ts>
    Data0D<T1, Ts...> readDataSet0D(String const& name) const noexcept(false);

    /*
     *  WRITE ATTRIBUTE
     */

    /**
     * @brief Delegates the function call to Node::writeVersionAttribute
     * @param string Attribute name
     * @param version Version to write
     * @return This
     */
    Group const& writeVersionAttribute(String const& string = versionAttributeName(),
                                       Version version = Version::current()
                                       ) const noexcept(false)
    {
        Node::writeVersionAttribute(string, version);
        return *this;
    }

    /**
     * @brief Delegates the function call to Node::writeAttribute
     * @param name Name of attribute
     * @param data Data to write
     * @return This
     */
    template <typename T>
    Group const& writeAttribute(String const& name, T&& data
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
    Group const& writeAttribute0D(String const& name, Container&& data
                                  ) const noexcept(false)
    {
        Node::writeAttribute0D(name, std::forward<Container>(data));
        return *this;
    }

    /*
     *  FIND CHILD NODES
     */

    /**
     * @brief Returns a list of all child nodes.
     * @param iterType Iteration type (i.e. direct only or all recursive)
     * @param iterFilter Filter to use. If a filter is set, all info structs are
     * guranteed to be of the type specified.
     * @param indexType Indexing type
     * @param indexOrder Order of the list
     * @return Node info structs
     */
    Vector<NodeInfo> findChildNodes(IterationType iterType = FindDirectOnly,
                                    IterationFilter iterFilter = NoFilter,
                                    IterationIndex iterIndex = IndexName,
                                    IterationOrder iterOrder = NativeOrder
                                    ) const noexcept;

    /**
     * @brief Iterates over each child node and calls a function on it.
     * @param iterFunction Functon to invoke.
     * @param iterType Iteration type (i.e. direct only or all recursive)
     * @param iterFilter Filter to use. If a filter is set, all info structs are
     * guranteed to be of the type specified.
     * @param indexType Indexing type
     * @param indexOrder Order of the list
     * @return Error
     */
    herr_t iterateChildNodes(NodeIterationFunction iterFunction,
                             IterationType iterType = FindDirectOnly,
                             IterationFilter iterFilter = NoFilter,
                             IterationIndex iterIndex = IndexName,
                             IterationOrder iterOrder = NativeOrder
                             ) const noexcept;

    /**
     * @brief Returns the info struct of the node specified. May throw.
     * @param name path to the node
     * @return Attribute info struct
     */
    NodeInfo nodeInfo(String path) const noexcept(false);

    /// swaps all members
    void swap(Group& other) noexcept;

private:

    /// group id
    IdComponent<H5I_GROUP> m_id;

    friend class Reference;
};


namespace details
{

template <typename... Ts>
inline DataSet
writeDataSetHelper(Group const& obj,
                   String const& name,
                   details::AbstractData<Ts...> const& data) noexcept(false)
{
    auto dset = obj.createDataSet(name, data.dataType(), data.dataSpace());

    if (!dset.write(data))
    {
        throw DataSetException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to write data to dataset '" +
            name.toStdString() + '\''
        };
    }

    return dset;
}

template <typename Tdata, typename... Ts>
inline Tdata
readDataSetHelper(Group const& obj, String const& name) noexcept(false)
{
    auto dset = obj.openDataSet(name);

    Tdata data;
    data.setTypeNames(dset.dataType());

    if (!dset.read(data))
    {
        throw DataSetException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to read data from dataset '" +
            name.toStdString() + '\''
        };
    }

    return data;
}

} // namespace details

template <typename Container, typename if_container,
          traits::if_has_not_template_type<Container>>
inline DataSet
Group::writeDataSet(String const& name,
                    Container&& data) const noexcept(false)
{
    return details::writeDataSetHelper(
                *this, name, makeData(std::forward<Container>(data)));
}

template <typename T>
inline DataSet
Group::writeDataSet(String const& name,
                    details::AbstractData<T> const& data) const noexcept(false)
{
    return details::writeDataSetHelper(*this, name, data);
}

template <typename Container, traits::if_has_not_template_type<Container>>
inline DataSet
Group::writeDataSet0D(String const& name,
                      Container&& data) const noexcept(false)
{
    return details::writeDataSetHelper(
                *this, name, makeData0D(std::forward<Container>(data)));
}

template <typename T1, typename... Ts>
inline Data<T1, Ts...>
Group::readDataSet(String const& name) const noexcept(false)
{
    return details::readDataSetHelper<Data<T1, Ts...>>(*this, name);
}

template <typename T1, typename... Ts>
inline Data0D<T1, Ts...>
Group::readDataSet0D(String const& name) const noexcept(false)
{
    return details::readDataSetHelper<Data0D<T1, Ts...>>(*this, name);
}

} // namespace GenH5

inline void
swap(GenH5::Group& a, GenH5::Group& b) noexcept
{
    a.swap(b);
}

#endif // GENH5_GROUP_H
