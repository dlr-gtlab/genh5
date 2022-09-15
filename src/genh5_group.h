/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_GROUP_H
#define GENH5_GROUP_H

#include "genh5_node.h"

#include "genh5_datasetcproperties.h"
#include "genh5_utils.h"
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
    Group(std::shared_ptr<File> file, H5::Group group);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::Group const& toH5() const noexcept;

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
     * @brief explicitly closes the resource handle
     */
    void close();

    // groups
    Group createGroup(String const& name) const noexcept(false);
    Group openGroup(String const& name) const noexcept(false);

    // datasets
    DataSet createDataset(String const& name,
                          DataType const& dtype,
                          DataSpace const& dspace,
                          Optional<DataSetCProperties> cProps = {}
                          ) const noexcept(false);
    DataSet openDataset(String const& name) const noexcept(false);

    // find child nodes
    Vector<NodeInfo> findChildNodes(IterationType iterType = FindDirectOnly,
                                    IterationFilter iterFilter = NoFilter,
                                    IterationIndex iterIndex = IndexName,
                                    IterationOrder iterOrder = NativeOrder
                                    ) const noexcept;
    herr_t iterateChildNodes(NodeIterationFunction iterFunction,
                             IterationType iterType = FindDirectOnly,
                             IterationFilter iterFilter = NoFilter,
                             IterationIndex iterIndex = IndexName,
                             IterationOrder iterOrder = NativeOrder
                             ) const noexcept;

    NodeInfo nodeInfo(String path) const noexcept(false);

protected:

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    H5::H5Object const* toH5Object() const noexcept override;

private:

    /// hdf5 base instance
    H5::Group m_group{};

    friend class Reference;
};

} // namespace GenH5

#endif // GENH5_GROUP_H
