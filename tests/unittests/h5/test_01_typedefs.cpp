/* GenH5
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 29.04.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include <genh5_typedefs.h>

#include <H5public.h>
#include <H5Tpublic.h>
#include <H5Rpublic.h>
#include <H5Opublic.h>
#include <H5Spublic.h>


TEST(Test_01, constant_reference_buffer_size)
{
    static_assert(GenH5::details::reference_buffer_size == H5R_REF_BUF_SIZE,
                  "reference_buffer_size is not same as H5R_REF_BUF_SIZE");
}

TEST(Test_01, constant_token_size)
{
    static_assert(GenH5::details::token_size == H5O_MAX_TOKEN_SIZE,
                  "token_size is not same as H5O_MAX_TOKEN_SIZE");
}

TEST(Test_01, symbol_varlen_t)
{
    static_assert(sizeof (GenH5::varlen_t) == sizeof(hvl_t),
                  "varlen_t is not binary compatible to hvl_t");
    static_assert(alignof (GenH5::varlen_t) == alignof(hvl_t),
                  "varlen_t is not binary compatible to hvl_t");

    static_assert(sizeof (GenH5::varlen_t::len) == sizeof(hvl_t::len),
                  "varlen_t is not binary compatible to hvl_t");
    static_assert(sizeof (GenH5::varlen_t::p) == sizeof(hvl_t::p),
                  "varlen_t is not binary compatible to hvl_t");

    static_assert(offsetof(GenH5::varlen_t, len) == offsetof(hvl_t, len),
                  "varlen_t is not binary compatible to hvl_t");
    static_assert(offsetof(GenH5::varlen_t,   p) == offsetof(hvl_t,   p),
                  "varlen_t is not binary compatible to hvl_t");
}

TEST(Test_01, symbol_reference_t)
{
    static_assert(sizeof (GenH5::reference_t) == sizeof(H5R_ref_t),
                  "reference_t is not binary compatible to H5R_ref_t");
    static_assert(alignof (GenH5::reference_t) == alignof(H5R_ref_t),
                  "reference_t is not binary compatible to H5R_ref_t");

    static_assert(sizeof (GenH5::reference_t::u) == sizeof(H5R_ref_t::u),
                  "reference_t is not binary compatible to H5R_ref_t");

    static_assert(sizeof (GenH5::reference_t::u.__data) == sizeof(H5R_ref_t::u.__data),
                  "reference_t is not binary compatible to H5R_ref_t");
    static_assert(sizeof (GenH5::reference_t::u.align) == sizeof(H5R_ref_t::u.align),
                  "reference_t is not binary compatible to H5R_ref_t");

    static_assert(offsetof(GenH5::reference_t, u.__data) == offsetof(H5R_ref_t, u.__data),
                  "reference_t is not binary compatible to H5R_ref_t");
    static_assert(offsetof(GenH5::reference_t, u.align) == offsetof(H5R_ref_t, u.align),
                  "reference_t is not binary compatible to H5R_ref_t");
}

TEST(Test_01, symbol_token_t)
{
    static_assert(sizeof (GenH5::token_t) == sizeof(H5O_token_t),
                  "token_t is not binary compatible to H5O_token_t");
    static_assert(alignof (GenH5::token_t) == alignof(H5O_token_t),
                  "token_t is not binary compatible to H5O_token_t");

    static_assert(sizeof (GenH5::token_t::__data) == sizeof(H5O_token_t::__data),
                  "token_t is not binary compatible to H5O_token_t");

    static_assert(offsetof(GenH5::token_t, __data) == offsetof(H5O_token_t, __data),
                  "token_t is not binary compatible to H5O_token_t");
}

TEST(Test_01, enum_SelectionOp)
{
    static_assert(sizeof (GenH5::SelectionOp) == sizeof(H5S_seloper_t),
                  "enum SelectionOp is not compatible to enum H5S_seloper_t");
    static_assert(alignof (GenH5::SelectionOp) == alignof(H5S_seloper_t),
                  "enum SelectionOp is not compatible to enum H5S_seloper_t");

    static_assert ((int)GenH5::SelectionOp::Error == H5S_seloper_t::H5S_SELECT_NOOP,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectSet == H5S_seloper_t::H5S_SELECT_SET,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectOr == H5S_seloper_t::H5S_SELECT_OR,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectAnd == H5S_seloper_t::H5S_SELECT_AND,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectXor == H5S_seloper_t::H5S_SELECT_XOR,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectNotB == H5S_seloper_t::H5S_SELECT_NOTB,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectNotA == H5S_seloper_t::H5S_SELECT_NOTA,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectAppend == H5S_seloper_t::H5S_SELECT_APPEND,
                  "dataspace selection types do not match");
    static_assert ((int)GenH5::SelectionOp::SelectPrepend == H5S_seloper_t::H5S_SELECT_PREPEND,
                  "dataspace selection types do not match");
}

TEST(Test_01, enum_DataSpaceClass)
{
    static_assert(sizeof (GenH5::DataSpaceClass) == sizeof(H5S_class_t),
                  "enum DataSpaceClass is not compatible to enum H5S_class_t");
    static_assert(alignof (GenH5::DataSpaceClass) == alignof(H5S_class_t),
                  "enum DataSpaceClass is not compatible to enum H5S_class_t");

    static_assert ((int)GenH5::DataSpaceClass::Error == H5S_class_t::H5S_NO_CLASS,
                  "dataspace class types do not match");
    static_assert ((int)GenH5::DataSpaceClass::Null == H5S_class_t::H5S_NULL,
                  "dataspace class types do not match");
    static_assert ((int)GenH5::DataSpaceClass::Scalar == H5S_class_t::H5S_SCALAR,
                  "dataspace class types do not match");
    static_assert ((int)GenH5::DataSpaceClass::Regular == H5S_class_t::H5S_SIMPLE,
                  "dataspace class types do not match");
}

TEST(Test_01, enum_DataTypeClass)
{
    static_assert(sizeof (GenH5::DataTypeClass) == sizeof(H5T_class_t),
                  "enum DataTypeClass is not compatible to enum H5T_class_t");
    static_assert(alignof (GenH5::DataTypeClass) == alignof(H5T_class_t),
                  "enum DataTypeClass is not compatible to enum H5T_class_t");

    static_assert ((int)GenH5::DataTypeClass::Error == H5T_class_t::H5T_NO_CLASS,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Int == H5T_class_t::H5T_INTEGER,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Float == H5T_class_t::H5T_FLOAT,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Time == H5T_class_t::H5T_TIME,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::String == H5T_class_t::H5T_STRING,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::BitField == H5T_class_t::H5T_BITFIELD,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Opaque == H5T_class_t::H5T_OPAQUE,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Compound == H5T_class_t::H5T_COMPOUND,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Reference == H5T_class_t::H5T_REFERENCE,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Enum == H5T_class_t::H5T_ENUM,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::VarLen == H5T_class_t::H5T_VLEN,
                  "datatype class types do not match");
    static_assert ((int)GenH5::DataTypeClass::Array == H5T_class_t::H5T_ARRAY,
                  "datatype class types do not match");
}

TEST(Test_01, enum_IdType)
{
    static_assert(sizeof (GenH5::DataTypeClass) == sizeof(H5T_class_t),
                  "enum DataTypeClass is not compatible to enum H5T_class_t");
    static_assert(alignof (GenH5::DataTypeClass) == alignof(H5T_class_t),
                  "enum DataTypeClass is not compatible to enum H5T_class_t");

    static_assert ((int)GenH5::IdType::Uninitialized == H5I_type_t::H5I_UNINIT,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::BadId == H5I_type_t::H5I_BADID,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::File == H5I_type_t::H5I_FILE,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::Group == H5I_type_t::H5I_GROUP,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::DataType == H5I_type_t::H5I_DATATYPE,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::DataSpace == H5I_type_t::H5I_DATASPACE,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::DataSet == H5I_type_t::H5I_DATASET,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::Map == H5I_type_t::H5I_MAP,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::Attribute == H5I_type_t::H5I_ATTR,
                  "id class type pace class types do not match");
    static_assert ((int)GenH5::IdType::PropertyList == H5I_type_t::H5I_GENPROP_LST,
                  "id class type pace class types do not match");
}

TEST(Test_01, enum_IterationIndex)
{
    static_assert ((int)GenH5::IterationIndex::IndexName == H5_index_t::H5_INDEX_NAME,
                  "index types do not match");
    static_assert ((int)GenH5::IterationIndex::IndexCreationOrder == H5_index_t::H5_INDEX_CRT_ORDER,
                  "index types do not match");
}

TEST(Test_01, enum_IterationOrder)
{
    static_assert ((int)GenH5::IterationOrder::NativeOrder == H5_ITER_NATIVE,
                  "order types do not match");
    static_assert ((int)GenH5::IterationOrder::AscendingOrder == H5_ITER_INC,
                  "order types do not match");
    static_assert ((int)GenH5::IterationOrder::DescendingOrder == H5_ITER_DEC,
                  "order types do not match");
}
