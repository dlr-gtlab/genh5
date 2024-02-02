/* GTlab - Gas Turbine laboratory
 * copyright 2009-2024 by DLR
 *
 *  Created on: 2.2.2024
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */


#ifndef GENH5_STATIC_H
#define GENH5_STATIC_H

#if GENH5_STATIC
#include <stdint.h>

/// include `hsize_t`
typedef unsigned long long hsize_t;
/// include `hssize_t`
typedef signed long long   hssize_t;
/// include `hid_t`
using hid_t = int64_t;
/// include `herr_t`
using herr_t = int;
#else
#include <H5public.h>
#include <H5Ipublic.h>
#include <H5Rpublic.h>
#include <H5Opublic.h>
#include <H5Tpublic.h>
#endif

namespace GenH5
{

#if GENH5_STATIC

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

#else

using varlen_t    = hvl_t;
using reference_t = H5R_ref_t;
using token_t     = H5O_token_t;

#endif

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

} // namespace GenH5

#endif // GENH5_STATIC_H
