/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 15.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_TYPES_H
#define GENH5_TYPES_H

#include "genh5_mpl.h"

#include <tuple>
#include <array>
#include <QVector>

#include <stdint.h>

/// include `hsize_t`
using hsize_t  = unsigned long long;
/// include `hssize_t`
using hssize_t =  signed long long;
/// include `hid_t`
using hid_t  = int64_t;
/// include `herr_t`
using herr_t = int;

namespace GenH5
{

namespace details
{

constexpr size_t reference_buffer_size = 64;
constexpr size_t token_size            = 16;

}

/// wrapper for `hvl_t`
struct varlen_t
{
    size_t len;
    void * p;
};

/// wrapper for `H5R_ref_t`
struct reference_t
{
    union
    {
        uint8_t __data[details::reference_buffer_size];
        int64_t align;
    } u;
};

/// wrapper for `H5O_token_t`
struct token_t
{
    uint8_t __data[details::token_size];
};

/// wrapper for `H5S_seloper_t`
enum class SelectionOp
{
    Error          = -1,
    SelectSet      =  0,
    SelectOr       =  1,
    SelectAnd      =  2,
    SelectXor      =  3,
    SelectNotB     =  4,
    SelectNotA     =  5,
    SelectAppend   =  6,
    SelectPrepend  =  7,
};

/// wrapper for `H5S_class_t`
enum class DataSpaceClass
{
    Error   = -1,
    Scalar  =  0,
    Regular =  1, // = Simple
    Null    =  2,
};

/// wrapper for `H5T_class_t`
enum class DataTypeClass
{
    Error     = -1,
    Int       =  0,
    Float     =  1,
    Time      =  2,
    String    =  3,
    BitField  =  4,
    Opaque    =  5,
    Compound  =  6,
    Reference =  7,
    Enum      =  8,
    VarLen    =  9,
    Array     = 10,
};

/// wrapper for `H5I_type_t`
enum class IdType
{
    Uninitialized = -2,
    BadId         = -1,
    File          =  1,
    Group         =  2,
    DataType      =  3,
    DataSpace     =  4,
    DataSet       =  5,
    Map           =  6,
    Attribute     =  7,
    PropertyList  = 11,
};

/**
 * @brief The IterationIndex enum. Can be used to specify the index type when
 * iterating over objects.
 */
enum IterationIndex
{
    IndexName          = 0, /// index by name
    IndexCreationOrder = 1, /// index by creation order
};

/**
 * @brief The IterationOrder enum. Can be used to specify the order in which
 * the objects.should be listed when iterating.
 */
enum IterationOrder
{
    NativeOrder     = 2, // no particular order
    AscendingOrder  = 0, // ascending
    DescendingOrder = 1, // descending
};

/**
 * @brief The IterationType enum. Used to specify the search depth when
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

// alias for vector type
template <typename T>
using Vector = QVector<T>;

// alias for strings
using String = QByteArray;

// alias for dimension vector type
using Dimensions = Vector<hsize_t>;

// alias for a compound type
template <typename ...Ts>
using Comp = std::tuple<Ts...>;

// alias for a reverse compound type
template <typename ...Ts>
using RComp = mpl::reversed_comp_t<Comp<Ts...>>;

// alias for a var len type
template<typename T>
using VarLen = Vector<T>;

// alias for a fixed length array type
template<typename T, size_t N>
using Array = std::array<T, N>;

// alias for compound member names
template <size_t N>
using CompoundNames = std::array<String, N>;

} // namespace GenH5

#endif // GENH5_TYPES_H
