/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_NODE_H
#define GENH5_NODE_H

#include "genh5_location.h"
#include "genh5_attribute.h"
#include "genh5_data.h"

#include <functional>


namespace GenH5
{
// forward decl
class Group;
class DataSet;
class Node;

struct Version;
struct NodeInfo;
struct AttributeInfo;

/// callback function type for iterating over attributes
using AttributeIterationFunction =
        std::function<herr_t(Node const&, AttributeInfo const&)>;

/**
 * @brief The IterationIndex enum. Can be used to specify the index type when
 * iterating over objects.
 */
enum IterationIndex
{
    IndexName = H5_INDEX_NAME, /// index by name
    IndexCreationOrder = H5_INDEX_CRT_ORDER /// index by creation order
};

/**
 * @brief The IterationOrder enum. Can be used to specify the order in which
 * the objects.should be listed when iterating.
 */
enum IterationOrder
{
    NativeOrder = H5_ITER_NATIVE, // no particular order
    AscendingOrder = H5_ITER_INC, // ascending
    DescendingOrder = H5_ITER_DEC // descending
};

/**
 * @brief The Node class
 */
class GENH5_EXPORT Node : public Location
{
public:

    /**
     * @brief returns whether the attribute exists at this node
     * @param name of the attribute
     * @return whether attibuet exists
     */
    bool hasAttribute(String const& name) const;

    /*
     * CREATE/OPEN ATTRIBUTE
     */

    /**
     * @brief Creates an attribute. The attribute has no values set. If the
     * attribute already exists, its datatype and dataspace are checked. If they
     * match the attribute will be opened else it will be overwritten.
     * @param name Name of the attribute
     * @param dtype Datatype of the attribute
     * @param dspace Dataspace of the attribute
     * @return Attribute
     */
    Attribute createAttribute(String const& name,
                              DataType const& dtype,
                              DataSpace const& dspace) const noexcept(false);

    /**
     * @brief Opens the attribute speicified.
     * @param name Name of the attribute
     * @return Attribute
     */
    Attribute openAttribute(String const& name) const noexcept(false);

    /**
     * @brief Opens the attribute speicified by its path and name.
     * @param name Name of the attribute
     * @return Attribute
     */
    Attribute openAttribute(String const& path,
                            String const& name) const noexcept(false);

    /*
     *  READ/WRITE ATTRIBUTE
     */

    /**
     * @brief High level method for creating and writing to an attribute.
     * @param name Name of the attribute
     * @return data Data to write
     * @return This
     */
    template <typename Container,
              typename if_container = traits::value_t<Container>,
              traits::if_has_not_template_type<Container> = true>
    Node const& writeAttribute(String const& name, Container&& data
                               ) const noexcept(false);

    /**
     * @brief Overload.
     * @param name Name of the attribute
     * @return data Data to write
     * @return This
     */
    template <typename T>
    Node const& writeAttribute(String const& name,
                               details::AbstractData<T> const& data
                               ) const noexcept(false);

    /**
     * @brief High level method for creating and writing to an attribute.
     * Cannot be used to write an object of type Data0D.
     * @param name Name of the attribute
     * @return data 0D Data to write
     * @return This
     */
    template <typename Container,
              traits::if_has_not_template_type<Container> = true>
    Node const& writeAttribute0D(String const& name, Container&& data
                                 ) const noexcept(false);

    /**
     * @brief High level method for opening and reading data from an attribute.
     * @param name Name of the attribute
     * @return Data read
     */
    template <typename T1, typename... Ts>
    Data<T1, Ts...> readAttribute(String const& name) const noexcept(false);

    /**
     * @brief High level method for opening and reading 0d data from an
     * attribute.
     * @param name Name of the attribute
     * @return Data read
     */
    template <typename T1, typename... Ts>
    Data0D<T1, Ts...> readAttribute0D(String const& name) const noexcept(false);

    /*
     *  VERSION ATTRIBUTE
     */

    // version attribute
#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    [[deprecated("use Node::versionAttributeName instead")]]
    static String versionAttrName();
#endif
    /**
     * @brief Returns the default name of a version attribute
     * @return Default name
     */
    static String versionAttributeName();

    /**
     * @brief Whether the node has the version attribute specified. By default
     * checks the library version attribute
     * @param string Attribute name
     * @return Does the version attribute exist
     */
    bool hasVersionAttribute(String const& string = versionAttributeName()) const;

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    [[deprecated("use Node::writeVersionAttribute instead")]]
    bool createVersionAttribute() const noexcept(false);
#endif

    /**
     * @brief Creates a attribute containing the version specified.
     * Can be called to update the version attribute. By default creates the
     * library version attribute.
     * @param string Attribute name
     * @param version Version to write
     * @return This
     */
    Node const& writeVersionAttribute(String const& string = versionAttributeName(),
                                      Version version = Version::current()
                                      ) const noexcept(false);

    /**
     * @brief Attempts to read the version attribute specified and returns
     * its value. By default reads the library version attribute.
     * @param string Attribute name
     * @return Version
     */
    Version readVersionAttribute(String const& string = versionAttributeName()
                                 ) const noexcept(false);

    /*
     *  FIND ATTRIBUTE
     */

    /**
     * @brief Returns a list of all attributes of this Node.
     * @param indexType Indexing type
     * @param indexOrder Order of the list
     * @return Attribute info structs
     */
    Vector<AttributeInfo> findAttributes(IterationIndex indexType = IndexName,
                                         IterationOrder indexOrder = NativeOrder
                                         ) const noexcept;
    /**
     * @brief Overload. Returns a list of all attributes of the Node
     * specified by path.
     * @param path Path to attribute
     * @param indexType Indexing type
     * @param indexOrder Order of the list
     * @return Attribute info structs
     */
    Vector<AttributeInfo> findAttributes(String const& path,
                                         IterationIndex indexType = IndexName,
                                         IterationOrder indexOrder = NativeOrder
                                         ) const noexcept;

    /**
     * @brief Iterates over each attribute of this Node and calls a function on
     * it.
     * @param iterFunction Functon to invoke.
     * @param indexType Indexing type
     * @param indexOrder Order
     * @return Error
     */
    herr_t iterateAttributes(AttributeIterationFunction iterFunction,
                             IterationIndex indexType = IndexName,
                             IterationOrder indexOrder = NativeOrder
                             ) const noexcept;
    /**
     * @brief Overload. Iterates over each attribute of the Node specified by
     * path and calls a function on it.
     * @param path Path to attribute
     * @param iterFunction Functon to invoke.
     * @param indexType Indexing type
     * @param indexOrder Order
     * @return Error
     */
    herr_t iterateAttributes(String const& path,
                             AttributeIterationFunction iterFunction,
                             IterationIndex indexType = IndexName,
                             IterationOrder indexOrder = NativeOrder
                             ) const noexcept;

    /*
     *  ATTRIBUTE INFO
     */

    /**
     * @brief Returns the info struct of the attribute specified. May throw.
     * @param name Name of the attribute
     * @return Attribute info struct
     */
    AttributeInfo attributeInfo(String const& name) const noexcept(false);

    /**
     * @brief Returns the info struct of the attribute specified by path and
     * its name. May throw.
     * @param name Name of the attribute
     * @return Attribute info struct
     */
    AttributeInfo attributeInfo(String const& path,
                                String const& name) const noexcept(false);

protected:

    /**
     * @brief GenH5Leaf
     */
    explicit Node(std::shared_ptr<File> file = {}) noexcept;

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    virtual H5::H5Object const* toH5Object() const noexcept = 0;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    H5::H5Location const* toH5Location() const noexcept override;
};


/**
 * @brief The NodeInfo struct. Contains basic information of a node and can be
 * used to instantiate a desired node type.
 */
struct GENH5_EXPORT NodeInfo
{
    /// relative path
    String path{};
    /// object type
    ObjectType type{H5I_BADID};
    /// creation order
    int64_t corder{-1};
    /// object token
    H5O_token_t token{};

    /**
     * @brief Whether the nodeinfo belongs to a dataset
     * @return Is datset
     */
    constexpr bool isDataSet() const { return type == H5I_DATASET; }
    /**
     * @brief  Whether the nodeinfo belongs to a dataset
     * @return Is group
     */
    constexpr bool isGroup() const { return type == H5I_GROUP; }

    /**
     * @brief Constructs a dataset object from the node location.
     * Must be a Dataset to succeed.
     * @param parent Parent object
     * @return Dataset
     */
    DataSet toDataSet(Group const& parent) const noexcept(false);

    /**
     * @brief Constructs a group object from the node location.
     * Must be Group to succeed.
     * @param parent Parent object
     * @return Group
     */
    Group toGroup(Group const& parent) const noexcept(false);

    /**
     * @brief Convenience function to construct an abstract node object on
     * the heap.
     * @param parent Parent object
     * @return Group or Dataset
     */
    std::unique_ptr<Node> toNode(Group const& parent) const noexcept(false);
};


struct GENH5_EXPORT AttributeInfo
{
    /// name
    String name{};
    /// creation order
    int64_t corder{-1};

    /**
     * @brief Constructs an attribute object from the node location.
     * @param parent Parent object
     * @return Attribute
     */
    Attribute toAttribute(Node const& parent) const noexcept(false);

    /**
     * @brief Constructs an attribute object from the node location.
     * @param object Object
     * @param path Parent object
     * @return Attribute
     */
    Attribute toAttribute(Node const& object,
                          String const& path) const noexcept(false);
};

namespace details
{

template <typename... Ts>
inline Node const&
writeAttributeHelper(Node const& obj,
                     String const& name,
                     details::AbstractData<Ts...> const& data) noexcept(false)
{
    auto attr = obj.createAttribute(name, data.dataType(), data.dataSpace());

    if (!attr.write(data))
    {
        throw GenH5::AttributeException{"Failed to write data to attribute!"};
    }

    return obj;
}

template <typename Tdata, typename... Ts>
inline Tdata
readAttributeHelper(Node const& obj, String const& name) noexcept(false)
{
    auto attr = obj.openAttribute(name);

    Tdata data;
    data.setTypeNames(attr.dataType());

    if (!attr.read(data))
    {
        throw GenH5::AttributeException{"Failed to read data from attribute!"};
    }

    return data;
}

} // namespace details

template <typename Container, typename if_container,
          traits::if_has_not_template_type<Container>>
inline Node const&
Node::writeAttribute(String const& name,
                     Container&& data) const noexcept(false)
{
    return details::writeAttributeHelper(
                *this, name, makeData(std::forward<Container>(data)));
}

template <typename T>
inline Node const&
Node::writeAttribute(String const& name,
                     details::AbstractData<T> const& data) const noexcept(false)
{
    return details::writeAttributeHelper(*this, name, data);
}

template <typename Container, traits::if_has_not_template_type<Container>>
inline Node const&
Node::writeAttribute0D(String const& name,
                       Container&& data) const noexcept(false)
{
    return details::writeAttributeHelper(
                *this, name, makeData0D(std::forward<Container>(data)));
}

template <typename T1, typename... Ts>
inline Data<T1, Ts...>
Node::readAttribute(String const& name) const noexcept(false)
{
    return details::readAttributeHelper<Data<T1, Ts...>>(*this, name);
}

template <typename T1, typename... Ts>
inline Data0D<T1, Ts...>
Node::readAttribute0D(String const& name) const noexcept(false)
{
    return details::readAttributeHelper<Data0D<T1, Ts...>>(*this, name);
}

} // namespace GenH5

#endif // GENH5_NODE_H
