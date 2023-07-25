/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 29.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_PRIVATE_H
#define GENH5_PRIVATE_H

#include "genh5_group.h"

#include "H5Ipublic.h"
#include "H5Apublic.h"

#include <utility>

namespace GenH5
{

namespace details
{

/// Helper function for creating T
template <typename T,
          typename Ex,
          typename Functor,
          typename CharType>
inline T
make(Functor const& functor, CharType&& msg)
{
    hid_t id = functor();
    if (id < 0)
    {
        throw Ex{std::forward<CharType>(msg)};
    }
    return T::fromId(id);
}

template <typename Functor>
inline GenH5::String
getName(hid_t id, Functor const& getNameFunctor)
{
    if (!GenH5::isValidId(id)) return {};

    std::vector<char> buffer(32);
    std::fill(std::begin(buffer), std::end(buffer), '\0');
    size_t bufferLen = buffer.size();

    auto acutalLen =
        numeric_cast<size_t>(getNameFunctor(id, bufferLen, buffer.data()));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(bufferLen);
        getNameFunctor(id, bufferLen, buffer.data());
    }

    // remove of excess whitespaces and trailing '\0'
    auto iter = std::find(std::begin(buffer), std::end(buffer), '\0');
    return std::string{std::begin(buffer), iter};
}

} // namespace details

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
    info.path = String{nodeName};
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
    data->nodes->push_back(std::move(info));
    return 0;
}

struct IterForeachNodeData
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

    auto* data = static_cast<IterForeachNodeData*>(dataPtr);
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

inline AttributeInfo
getAttributeInfo(char const* attrName, H5A_info_t const* attrInfo)
{
    assert(attrName);
    assert(attrInfo);

    AttributeInfo info;
    info.name = String{attrName};
    info.corder = attrInfo->corder_valid ? attrInfo->corder : -1;
    return info;
}

inline herr_t
accumulateAttributes(hid_t /*locId*/, char const* attrName,
                     H5A_info_t const* attrInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* nodes = static_cast<Vector<AttributeInfo>*>(dataPtr);
    nodes->push_back(getAttributeInfo(attrName, attrInfo));
    return 0;
}

struct IterForeachAttrData
{
    Node const* parent{};
    AttributeIterationFunction* f{};
};

inline herr_t
foreachAttribute(hid_t /*locId*/, char const* attrName,
                 H5A_info_t const* attrInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* data = static_cast<IterForeachAttrData*>(dataPtr);
    assert(data->parent);
    assert(data->f);

    AttributeInfo info = getAttributeInfo(attrName, attrInfo);

    return (*data->f)(*data->parent, info);
}

} // namespae alg

} // namespace GenH5

#endif // GENH5_PRIVATE_H
