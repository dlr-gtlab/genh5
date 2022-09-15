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

#include <functional>


namespace GenH5
{
// forward decl
class Group;
class DataSet;
class DataType;
class DataSpace;
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

    // attributes
    Attribute createAttribute(String const& name,
                              DataType const& dtype,
                              DataSpace const& dspace) const noexcept(false);
    Attribute openAttribute(String const& name) const noexcept(false);
    Attribute openAttribute(String const& path,
                            String const& name) const noexcept(false);

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
     * @brief Whether the node has a version attribute
     * @return Does the version attribute exist
     */
    bool hasVersionAttribute() const;
    /**
     * @brief Creates a attribute containing the version of this
     * library. Can be called to update the version attribute.
     * @return success
     */
    bool createVersionAttribute() const noexcept(false);
    /**
     * @brief Attempts to read the version attribute and returns its value.
     * @return Version
     */
    Version readVersionAttribute() const noexcept(false);

    // find attributes
    Vector<AttributeInfo> findAttributes(IterationIndex indexType = IndexName,
                                         IterationOrder indexOrder = NativeOrder
                                         ) const noexcept;
    Vector<AttributeInfo> findAttributes(String const& path,
                                         IterationIndex indexType = IndexName,
                                         IterationOrder indexOrder = NativeOrder
                                         ) const noexcept;
    herr_t iterateAttributes(AttributeIterationFunction iterFunction,
                             IterationIndex indexType = IndexName,
                             IterationOrder indexOrder = NativeOrder
                             ) const noexcept;
    herr_t iterateAttributes(String const& path,
                             AttributeIterationFunction iterFunction,
                             IterationIndex indexType = IndexName,
                             IterationOrder indexOrder = NativeOrder
                             ) const noexcept;

    AttributeInfo attributeInfo(String const& name) const noexcept(false);
    AttributeInfo attributeInfo(String const& path,
                                String const& name) const noexcept(false);

protected:

    /**
     * @brief GenH5Leaf
     */
    Node(std::shared_ptr<File> file = {}) noexcept;

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
     * Must be Dataset to succeed.
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

} // namespace GenH5

#endif // GENH5_NODE_H
