/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_group.h"
#include "genh5_file.h"
#include "genh5_private.h"

#include "H5Gpublic.h"
#include "H5Ppublic.h"

GenH5::Group::Group() = default;

GenH5::Group::Group(const File& file)
{
    if (!file.isValid())
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR()
            "Opening root group failed (invalid file)"
        };
    }

    m_id = H5Gopen(file.id(), "/", H5P_DEFAULT);

    if (m_id < 0)
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to open root group (path: )" +
            file.filePath().toStdString() + ")"
        };
    }
}

GenH5::Group::Group(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

hid_t
GenH5::Group::id() const noexcept
{
    return m_id;
}

void
GenH5::Group::deleteLink() noexcept(false)
{
    // returns error type
    if (H5Ldelete(file().id(), path().constData(), H5P_DEFAULT) < 0)
    {
        throw LocationException{
            GENH5_MAKE_EXECEPTION_STR() "Deleting group '" +
            path().toStdString() + "' failed"
        };
    }
    close();
}

void
GenH5::Group::deleteRecursively() noexcept(false)
{
    // sub groups and datasets
    for (auto& node : findChildNodes())
    {
        if (node.isGroup())
        {
            node.toGroup(*this).deleteRecursively();
        }
        else if (node.isDataSet())
        {
            node.toDataSet(*this).deleteRecursively();
        }
    }

    // attributes
    for (auto& attr : findAttributes())
    {
        attr.toAttribute(*this).deleteLink();
    }

    // this
    deleteLink();
}

void
GenH5::Group::close()
{
    m_id.dec();
}

GenH5::Group
GenH5::Group::createGroup(String const& name) const noexcept(false)
{
    if (!isValid())
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR() "Creating group '" +
            name.toStdString() + "' failed (invalid parent)"
        };
    }

    // create new group
    if (!exists(name))
    {
        hid_t group = H5Gcreate(m_id, name.constData(),
                                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        if (group < 0)
        {
            throw GroupException{
                GENH5_MAKE_EXECEPTION_STR() "Failed to create group '" +
                name.toStdString() + '\''
            };
        }

        auto cleanup = finally(H5Gclose, group);
        Q_UNUSED(cleanup)

        return Group{group};
    }

    // open existing group
    return openGroup(std::move(name));
}

GenH5::Group
GenH5::Group::openGroup(String const& name) const noexcept(false)
{
    if (!isValid())
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR() "Opening group '" +
            name.toStdString() + "' failed (invalid parent)"
        };
    }

    hid_t group = H5Gopen(m_id, name.constData(), H5P_DEFAULT);
    if (group < 0)
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to open group '" +
            name.toStdString() + '\''
        };
    }

    auto cleanup = finally(H5Gclose, group);
    Q_UNUSED(cleanup)

    return Group{group};
}

GenH5::DataSet
GenH5::Group::createDataSet(String const& name,
                            DataType const& dtype,
                            DataSpace const& dspace,
                            Optional<DataSetCProperties> properties) const
{
    if (!isValid())
    {
        throw DataSetException{
            GENH5_MAKE_EXECEPTION_STR() "Creating dataset '" +
            name.toStdString() + "' failed (invalid parent)"
        };
    }

    // chunk dataset by default
    if (properties.isDefault() && !dspace.isScalar() && dspace.size() > 0)
    {
        properties = DataSetCProperties::autoChunked(dspace);
    }

    // create new dataset
    if (!exists(name))
    {
        hid_t dset = H5Dcreate(m_id, name.constData(), dtype.id(), dspace.id(),
                               H5P_DEFAULT, properties->id(), H5P_DEFAULT);
        if (dset < 0)
        {
            throw DataSetException{
                GENH5_MAKE_EXECEPTION_STR() "Failed to create dataset '" +
                name.toStdString() + '\''
            };
        }

        auto cleanup = finally(H5Dclose, dset);
        Q_UNUSED(cleanup)

        return DataSet{dset};
    }

    // open existing dataset
    auto dset = openDataSet(name);

    // check if datatype is equal
    if (dset.dataType() == dtype)
    {
        // check if dataspace is equal or resizing succeded
        if (dset.dataSpace() == dspace || dset.resize(dspace.dimensions()))
        {
            return dset;
        }
    }

    // dataset cannot be resized and must be deleted
    log::ErrStream() << GENH5_MAKE_EXECEPTION_STR()
                        "Invalid memory layout, overwriting dataset: " << name;
    dset.deleteLink();

    return createDataSet(name, dtype, dspace, std::move(properties));
}

GenH5::DataSet
GenH5::Group::openDataSet(String const& name) const noexcept(false)
{
    if (!isValid())
    {
        throw DataSetException{
            GENH5_MAKE_EXECEPTION_STR() "Opening dataset '" +
            name.toStdString() + "' failed (invalid parent)"
        };
    }

    hid_t dset = H5Dopen(m_id, name.constData(), H5P_DEFAULT);
    if (dset < 0)
    {
        throw DataSetException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to open dataset '" +
            name.toStdString() + '\''
        };
    }

    auto cleanup = finally(H5Dclose, dset);
    Q_UNUSED(cleanup)

    return DataSet{dset};
}

namespace GenH5
{

namespace alg
{

inline NodeInfo getNodeInfo(hid_t groupId, char const* nodeName,
                            H5L_info_t const* nodeInfo)
{
    assert(nodeName);
    assert(nodeInfo);
    // open object to retrieve object type
    auto id = H5Oopen_by_token(groupId, nodeInfo->u.token);
    auto type = H5Iget_type(id);
    H5Oclose(id);

    NodeInfo info;
    info.path = QByteArray{nodeName};
    info.type = type;
    info.corder = nodeInfo->corder_valid ? nodeInfo->corder : -1;
    info.token = nodeInfo->u.token;
    return info;
}

struct IterAccumulateData
{
    Vector<NodeInfo>* nodes{};
    IterationFilter filter{};
};

inline herr_t
accumulateNodes(hid_t groupId, char const* nodeName,
                H5L_info_t const* nodeInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* data = static_cast<IterAccumulateData*>(dataPtr);
    assert(data->nodes);

    NodeInfo info = getNodeInfo(groupId, nodeName, nodeInfo);
    if (data->filter == FilterGroups && info.type != H5I_GROUP)
    {
        return 0;
    }
    if (data->filter == FilterDataSets && info.type != H5I_DATASET)
    {
        return 0;
    }
    *data->nodes << std::move(info);
    return 0;
}

struct IterForeachData
{
    Group const* parent{};
    NodeIterationFunction* f{};
    IterationFilter filter{};
};

inline herr_t
foreachNode(hid_t groupId, char const* nodeName,
            H5L_info_t const* nodeInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* data = static_cast<IterForeachData*>(dataPtr);
    assert(data->parent);
    assert(data->f);

    NodeInfo info = getNodeInfo(groupId, nodeName, nodeInfo);
    if (data->filter == FilterGroups && info.type != H5I_GROUP)
    {
        return 0;
    }
    if (data->filter == FilterDataSets && info.type != H5I_DATASET)
    {
        return 0;
    }
    return (*data->f)(*data->parent, info);
}

} // namespae alg

} // namespace GenH5

GenH5::Vector<GenH5::NodeInfo>
GenH5::Group::findChildNodes(IterationType iterType,
                             IterationFilter iterFilter,
                             IterationIndex iterIndex,
                             IterationOrder iterOrder) const noexcept
{
    Vector<NodeInfo> nodes;
    alg::IterAccumulateData data{&nodes, iterFilter};

    auto h5index = static_cast<H5_index_t>(iterIndex);
    auto h5order = static_cast<H5_iter_order_t>(iterOrder);

    if (iterType == FindDirectOnly)
    {
        hsize_t idx = 0;
        H5Literate(m_id, h5index, h5order, &idx, alg::accumulateNodes, &data);
    }
    else
    {
        H5Lvisit(m_id, h5index, h5order, alg::accumulateNodes, &data);
    }

    return nodes;
}

herr_t
GenH5::Group::iterateChildNodes(NodeIterationFunction iterFunction,
                                IterationType iterType,
                                IterationFilter iterFilter,
                                IterationIndex iterIndex,
                                IterationOrder iterOrder) const noexcept
{
    assert(iterFunction);

    alg::IterForeachData data{this, &iterFunction, iterFilter};
    herr_t error = 0;

    auto h5index = static_cast<H5_index_t>(iterIndex);
    auto h5order = static_cast<H5_iter_order_t>(iterOrder);

    if (iterType == FindDirectOnly)
    {
        hsize_t idx = 0;
        error = H5Literate(m_id, h5index, h5order, &idx,
                           alg::foreachNode, &data);
    }
    else
    {
        error = H5Lvisit(m_id, h5index, h5order,
                         alg::foreachNode, &data);
    }

    return error;
}

GenH5::NodeInfo
GenH5::Group::nodeInfo(String path) const noexcept(false)
{
    H5L_info_t info{};
    if (H5Lget_info(m_id, path.constData(), &info, H5P_DEFAULT))
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to retrieve node info for '" +
            path.toStdString() + '\''
        };
    }
    return alg::getNodeInfo(m_id, path.constData(), &info);
}

void
GenH5::Group::swap(Group& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
}
