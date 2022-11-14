/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_group.h"
#include "genh5_file.h"

#include <QDebug>


GenH5::Group::Group() = default;

GenH5::Group::Group(const File& file) :
    Node{nullptr}
{
    if (!file.isValid())
    {
        throw GroupException{"Opening root group failed! (Invalid file)"};
    }

    m_file = std::make_shared<File>(file);

    try
    {
        m_group = file.toH5().openGroup(QByteArrayLiteral("/").constData());
    }
    catch (H5::GroupIException const& e)
    {
        throw GroupException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Group::Group";
        throw GroupException{e.getCDetailMsg()};
    }
}

GenH5::Group::Group(std::shared_ptr<File> file, H5::Group group) :
    Node{std::move(file)},
    m_group{std::move(group)}
{ }

hid_t
GenH5::Group::id() const noexcept
{
    return m_group.getId();
}

void
GenH5::Group::deleteLink() noexcept(false)
{    
    // returns error type
    if (H5Ldelete(m_file->id(), name().constData(), H5P_DEFAULT))
    {
        throw LocationException{"Deleting group failed"};
    }
    close();
}

H5::H5Object const*
GenH5::Group::toH5Object() const noexcept
{
    return &m_group;
}

H5::Group const&
GenH5::Group::toH5() const noexcept
{
    return m_group;
}

void
GenH5::Group::close()
{
    m_group.close();
}

GenH5::Group
GenH5::Group::createGroup(String const& name) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw GroupException{"Creating group failed (Invalid parent"};
    }

    // create new group
    if (!parent.exists(name))
    {
        try
        {
            H5::Group group = parent.toH5().createGroup(name.constData());
            return Group{parent.file(), std::move(group)};
        }
        catch (H5::GroupIException const& e)
        {
            throw GroupException{e.getCDetailMsg()};
        }
        catch (H5::Exception const& e)
        {
            qCritical() << "HDF5: [EXCEPTION] Group::createGroup";
            throw GroupException{e.getCDetailMsg()};
        }
    }

    // open existing group
    return openGroup(std::move(name));
}

GenH5::Group
GenH5::Group::openGroup(String const& name) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw GroupException{"Opening group failed (Invalid parent"};
    }

    try
    {
        H5::Group group = parent.toH5().openGroup(name.constData());
        return Group{parent.file(), std::move(group)};
    }
    catch (H5::GroupIException const& e)
    {
        throw GroupException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Group::openGroup";
        throw GroupException{e.getCDetailMsg()};
    }
}

GenH5::DataSet
GenH5::Group::createDataset(String const& name,
                           DataType const& dtype,
                           DataSpace const& dspace,
                           Optional<DataSetCProperties> properties) const
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw DataSetException{"Opening dataset failed (Invalid parent"};
    }

    // chunk dataset by default
    if (properties.isDefault() && !dspace.isScalar() && dspace.size() > 0)
    {
        properties = DataSetCProperties::autoChunked(dspace);
    }

    // create new dataset
    if (!parent.exists(name))
    {
        try
        {
            H5::DataSet dset = parent.toH5().createDataSet(name.constData(),
                                                           dtype.toH5(),
                                                           dspace.toH5(),
                                                           properties->toH5());
            return DataSet{parent.file(), std::move(dset)};
        }
        catch (H5::DataSetIException const& e)
        {
            throw DataSetException{e.getCDetailMsg()};
        }
        catch (H5::Exception const& e)
        {
            qCritical() << "HDF5: [EXCEPTION] Group::createDataset";
            throw DataSetException{e.getCDetailMsg()};
        }
    }

    // open existing dataset
    auto dset = openDataset(name);

    // check if datatype is equal
    if (dset.dataType() == dtype)
    {
        // check if dataspace is equal or resize dataset
        if (dset.dataSpace() == dspace || dset.resize(dspace.dimensions()))
        {
            return dset;
        }
    }

    // dataset cannot be resized and must be deleted
    qWarning() << "HDF5: Invalid memory layout! Overwriting dataset! -" << name;
    dset.deleteLink();

    return createDataset(name, dtype, dspace, std::move(properties));
}

GenH5::DataSet
GenH5::Group::openDataset(String const& name) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw DataSetException{"Opening dataset failed (Invalid parent)"};
    }

    try
    {
        H5::DataSet dset = parent.toH5().openDataSet(name.constData());
        return DataSet{parent.file(), std::move(dset)};
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::GroupIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Group::openDataset";
        throw DataSetException{e.getCDetailMsg()};
    }
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
//    qDebug() << "NODE" << nodeName;
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
//    qDebug() << "NODE" << nodeName;
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
        H5Literate(id(), h5index, h5order, &idx, alg::accumulateNodes, &data);
    }
    else
    {
        H5Lvisit(id(), h5index, h5order, alg::accumulateNodes, &data);
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
        error = H5Literate(id(), h5index, h5order, &idx,
                           alg::foreachNode, &data);
    }
    else
    {
        error = H5Lvisit(id(), h5index, h5order,
                         alg::foreachNode, &data);
    }

    return error;
}

GenH5::NodeInfo
GenH5::Group::nodeInfo(String path) const noexcept(false)
{
    H5L_info_t info{};
    if (H5Lget_info(id(), path.constData(), &info, H5P_DEFAULT))
    {
        throw GroupException{"Failed to retrieve node info"};
    }
    return alg::getNodeInfo(id(), path.constData(), &info);
}
