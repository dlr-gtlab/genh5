/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 *  Created on: 29.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_PRIVATE_H
#define GENH5_PRIVATE_H

#include "genh5_group.h"

#include <H5Ipublic.h>
#include <H5Lpublic.h>
#include <H5Apublic.h>

#include <algorithm>
#include <type_traits>
#include <utility>

#include <QVarLengthArray>
#include <QDebug>

namespace GenH5
{

// HDF5 library compatibility
namespace compat
{

static_assert(sizeof(hsize_t) == sizeof(::hsize_t),
              "GenH5 and HDF5 hsize_t type must have the same size");
static_assert(std::is_signed<hsize_t>::value == std::is_signed<::hsize_t>::value,
              "GenH5 and HDF5 hsize_t type must have the same signedness");

static_assert(sizeof(hssize_t) == sizeof(::hssize_t),
              "GenH5 and HDF5 hssize_t type must have the same size");
static_assert(std::is_signed<hssize_t>::value == std::is_signed<::hssize_t>::value,
              "GenH5 and HDF5 hssize_t type must have the same signedness");

// helper structs to resolve whether we need a conversion or not
template<typename T>
struct resolve_dimensions
{
    using type = QVarLengthArray<::hsize_t, 16>;
};

template<>
struct resolve_dimensions<hsize_t>
{
    using type = Dimensions;
};

using H5Dimensions = typename resolve_dimensions<::hsize_t>::type;

inline constexpr bool h5DimensionsMatch = std::is_same_v<H5Dimensions, Dimensions>;

template<typename Dim>
auto toH5Dimensions(Dim&& dimensions)
{
    if constexpr (h5DimensionsMatch)
    {
        return std::forward<Dim>(dimensions);
    }
    else
    {
        H5Dimensions result(dimensions.size());
        std::transform(dimensions.cbegin(),
                       dimensions.cend(),
                       result.begin(),
                       [](hsize_t dimension) {
            return static_cast<::hsize_t>(dimension);
         });
        return result;
    }
}

template<typename Dim>
auto fromH5Dimensions(Dim&& dimensions)
{
    if constexpr (h5DimensionsMatch)
    {
        return std::forward<Dim>(dimensions);
    }
    else
    {
        Dimensions result(dimensions.size());
        std::transform(dimensions.cbegin(),
                       dimensions.cend(),
                       result.begin(),
                       [](hsize_t dimension) {
            return static_cast<hsize_t>(dimension);
        });
        return result;
    }
}

} // namespace compat

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
    if (!GenH5::isValidId(id))
    {
        return {};
    }

    String buffer{32, ' '};
    size_t bufferLen = static_cast<size_t>(buffer.size());

    // length required to store the string (with null termination, hence +1)
    auto acutalLen = 1 +
            static_cast<size_t>(getNameFunctor(id, bufferLen, buffer.data()));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen;
        buffer.resize(static_cast<int>(bufferLen));
        getNameFunctor(id, bufferLen, buffer.data());
    }

    // remove of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
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
    info.path = QByteArray{nodeName};
    info.type = static_cast<IdType>(type);
    info.corder = nodeInfo->corder_valid ? nodeInfo->corder : -1;
    memcpy(info.token.__data, nodeInfo->u.token.__data, sizeof(nodeInfo->u.token.__data));
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
    if (data->filter == FilterGroups && info.type != IdType::Group)
    {
        return 0;
    }
    if (data->filter == FilterDataSets && info.type != IdType::DataSet)
    {
        return 0;
    }
    *data->nodes << std::move(info);
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
    if (data->filter == FilterGroups && info.type != IdType::Group)
    {
        return 0;
    }
    if (data->filter == FilterDataSets && info.type != IdType::DataSet)
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
    info.name = QByteArray{attrName};
    info.corder = attrInfo->corder_valid ? attrInfo->corder : -1;
    return info;
}

inline herr_t
accumulateAttributes(hid_t /*locId*/, char const* attrName,
                     H5A_info_t const* attrInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* nodes = static_cast<Vector<AttributeInfo>*>(dataPtr);
    *nodes << getAttributeInfo(attrName, attrInfo);
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
