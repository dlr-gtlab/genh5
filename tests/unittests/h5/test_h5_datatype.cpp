/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_datatype.h"
#include "genh5_conversion.h"
#include "genh5_exception.h"

#include <QPoint>
#include <QPointF>

/// This is a test fixture that does a init for each test
class TestH5DataType : public testing::Test
{
protected:
    virtual void SetUp() override
    {

    }

    // invalid
    GenH5::DataType dtypeEmpty{};
    // int
    GenH5::DataType dtypeInt = GenH5::dataType<int>();
    // double
    GenH5::DataType dtypeDouble = GenH5::dataType<double>();
    // string
    GenH5::DataType dtypeString = GenH5::dataType<QString>();
};

TEST_F(TestH5DataType, isValid)
{
    EXPECT_FALSE(dtypeEmpty.isValid());

    EXPECT_TRUE(dtypeInt.isValid());
    EXPECT_TRUE(dtypeDouble.isValid());
    EXPECT_TRUE(dtypeString.isValid());
}

TEST_F(TestH5DataType, equal)
{
    EXPECT_FALSE(dtypeInt == dtypeDouble);
    EXPECT_FALSE(dtypeInt == dtypeEmpty);
    EXPECT_FALSE(dtypeDouble == dtypeString);
    EXPECT_FALSE(dtypeDouble == dtypeEmpty);
    EXPECT_FALSE(dtypeString == dtypeInt);
    EXPECT_FALSE(dtypeString == dtypeEmpty);

    // the same objects should be equal
    EXPECT_TRUE(dtypeEmpty == dtypeEmpty);
    EXPECT_TRUE(dtypeInt == dtypeInt);
    EXPECT_TRUE(dtypeDouble == dtypeDouble);
    EXPECT_TRUE(dtypeString == dtypeString);
}

TEST_F(TestH5DataType, predefinedTypes)
{
    // all predefined types should be valid
    EXPECT_TRUE(GenH5::DataType::VarString.isValid());
    EXPECT_TRUE(GenH5::DataType::Version.isValid());
    EXPECT_TRUE(GenH5::DataType::Version.isCompound());
    EXPECT_EQ(GenH5::DataType::Version.compoundMembers().size(), 3);

    EXPECT_TRUE(GenH5::DataType::Bool.isValid());
    EXPECT_TRUE(GenH5::DataType::Char.isValid());

    EXPECT_TRUE(GenH5::DataType::Int.isValid());
    EXPECT_TRUE(GenH5::DataType::Long.isValid());
    EXPECT_TRUE(GenH5::DataType::LLong.isValid());
    EXPECT_TRUE(GenH5::DataType::UInt.isValid());
    EXPECT_TRUE(GenH5::DataType::ULong.isValid());
    EXPECT_TRUE(GenH5::DataType::ULLong.isValid());

    EXPECT_TRUE(GenH5::DataType::Float.isValid());
    EXPECT_TRUE(GenH5::DataType::Double.isValid());
}

// declare datatype implementation for QPoint
GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using Tp = decltype (std::declval<QPoint>().x());
    return GenH5::dataType<Tp, Tp>({"xp", "yp"});
};

TEST_F(TestH5DataType, invalid)
{
    GenH5::DataType dtype;
    EXPECT_NO_THROW(dtype.compoundMembers());
    EXPECT_NO_THROW(dtype.arrayDimensions());
    EXPECT_THROW(dtype.superType(), GenH5::DataTypeException);
}

TEST_F(TestH5DataType, qpointDtype)
{
    auto type = GenH5::dataType<QPoint>();

    ASSERT_TRUE(type.isValid());
    EXPECT_TRUE(type.isCompound());

    // check if compound members are correct
    EXPECT_EQ(GenH5::dataType<QPoint>().compoundMembers().size(), 2);
    for (auto& m: type.compoundMembers())
    {
        EXPECT_TRUE(m.type.isValid());
        EXPECT_FALSE(m.type.isCompound());
        EXPECT_TRUE(m.type == GenH5::dataType<int>());
    }


    EXPECT_TRUE((type.size() == GenH5::dataType<int, int>().size()));
    EXPECT_TRUE((type.type() == GenH5::dataType<int, int>().type()));
    // memory layout is equal to compound type of two ints
    EXPECT_TRUE((GenH5::dataType<QPoint>() == GenH5::dataType<int, int>()));
}

TEST_F(TestH5DataType, fromTemplateSimple)
{
    // test simple types
    EXPECT_TRUE(GenH5::dataType<int>().isValid());
    EXPECT_TRUE(GenH5::dataType<long>().isValid());
    EXPECT_TRUE(GenH5::dataType<float>().isValid());
    EXPECT_TRUE(GenH5::dataType<double>().isValid());
    EXPECT_TRUE(GenH5::dataType<char const*>().isValid());
    EXPECT_TRUE(GenH5::dataType<char*>().isValid());
    EXPECT_TRUE(GenH5::dataType<char>().isValid());
    EXPECT_TRUE(GenH5::dataType<QString>().isValid());
    EXPECT_TRUE(GenH5::dataType<QByteArray>().isValid());

    EXPECT_TRUE(GenH5::dataType<QPoint>().isValid());
}

TEST_F(TestH5DataType, fromTemplateCompound)
{
    // test some compound types
    GenH5::DataType dtype;
    dtype = GenH5::dataType<int, QString>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<double, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<int, char const*>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<QByteArray, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<double, int, QString>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<QString, double, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<int, size_t, char const*>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GenH5::dataType<char, QPoint>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    /* WILL PROCDUCE BUILD ERRORS */
    // contains unsupported datatypes
//    EXPECT_FALSE(GenH5::dataType<bool>().isValid());
//    EXPECT_FALSE(GenH5::dataType<QPointF>().isValid());
//    EXPECT_FALSE((GenH5::dataType<double, bool>().isValid()));
//    EXPECT_FALSE((GenH5::dataType<int, QPointF, QString>().isValid()));
//    EXPECT_FALSE((GenH5::dataType<QPointF, bool, char>().isValid()));
}

TEST_F(TestH5DataType, fromTemplateVarLen)
{
    // types
    GenH5::DataType dtype;
    dtype = GenH5::dataType<GenH5::VarLen<int>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == GenH5::dataType<int>());

    dtype = GenH5::dataType<GenH5::VarLen<QByteArray>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == GenH5::dataType<QByteArray>());

    dtype = GenH5::dataType<GenH5::VarLen<QPoint>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == GenH5::dataType<QPoint>());

    // compound types
    dtype = GenH5::dataType<GenH5::VarLen<GenH5::Comp<double, QString>>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE((dtype.superType() ==
                 GenH5::dataType<GenH5::Comp<double, QString>>()));

    dtype = GenH5::dataType<GenH5::VarLen<GenH5::Comp<double, int, QPoint>>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE((dtype.superType() ==
                 GenH5::dataType<GenH5::Comp<double, int, QPoint>>()));
}

TEST_F(TestH5DataType, fromTemplateComplex)
{
    using CompT = GenH5::Comp<QPoint, QString>;
    using VarLenT = GenH5::VarLen<CompT>;
    using ArrayT = GenH5::Array<VarLenT, 42>;

    // create a complex type and check each member
    auto arrayType = GenH5::dataType<ArrayT>();
    EXPECT_TRUE(arrayType.isValid());
    EXPECT_TRUE(arrayType.isArray());
    EXPECT_TRUE(arrayType.arrayDimensions() == GenH5::Dimensions{42});

    auto varlenType =  GenH5::dataType<VarLenT>();
    EXPECT_TRUE(varlenType.isValid());
    EXPECT_TRUE(varlenType.isVarLen());

    auto compType =  GenH5::dataType<CompT>();
    EXPECT_TRUE(compType.isValid());
    EXPECT_TRUE(compType.isCompound());

    auto members = compType.compoundMembers();
    EXPECT_EQ(members.size(), 2);
    EXPECT_TRUE(members.at(0).type == GenH5::dataType<QPoint>());
    EXPECT_TRUE(members.at(1).type == GenH5::dataType<QString>());

    EXPECT_TRUE(varlenType.superType() == compType);

    EXPECT_TRUE(arrayType.superType() == varlenType);
}

TEST_F(TestH5DataType, array)
{
    GenH5::Dimensions dims{10, 2};
    auto arrayType = GenH5::DataType::array(GenH5::DataType::Char, dims);
    EXPECT_TRUE(arrayType.isValid());

    EXPECT_TRUE(arrayType.isArray());
    EXPECT_EQ(arrayType.arrayDimensions(), dims);
}

TEST_F(TestH5DataType, arrayInvalid)
{
    GenH5::Dimensions dims{10, 2};
    EXPECT_THROW(GenH5::DataType::array(GenH5::DataType{}, dims),
                 H5::DataTypeIException);

//    EXPECT_FALSE(arrayType.isValid());
//    EXPECT_FALSE(arrayType.isArray());
//    EXPECT_EQ(arrayType.arrayDimensions(), GenH5::Dimensions{});
}

struct Data
{
    int a;
    double b;
};

TEST_F(TestH5DataType, compound)
{
    GenH5::CompoundMembers members{
        {"a", offsetof(Data, a), GenH5::dataType<int>()},
        {"b", offsetof(Data, b), GenH5::dataType<double>()}
    };

    auto type = GenH5::DataType::compound(sizeof (Data), members);

    EXPECT_TRUE(type.isValid());
    EXPECT_TRUE(type == (GenH5::dataType<int, double>()));

    EXPECT_TRUE(type.isCompound());
    EXPECT_EQ(type.size(), sizeof(Data));

    auto members2 = type.compoundMembers();
    ASSERT_EQ(members2.length(), members.length());
    ASSERT_EQ(members2.length(), 2);
    EXPECT_TRUE(members2.at(0) == members.at(0));
    EXPECT_TRUE(members2.at(1) == members.at(1));
}

TEST_F(TestH5DataType, compoundInvalid)
{
    // empty member list
    auto type1 = GenH5::DataType::compound(sizeof (Data), {});
    EXPECT_TRUE(type1.isValid());
    EXPECT_TRUE(type1.isCompound());

    // empty data size
    EXPECT_THROW(GenH5::DataType::compound(0, {
        {"type 1", offsetof(Data, a), GenH5::DataType::Int},
        {"type 2", offsetof(Data, b), GenH5::DataType::Double}
    }), GenH5::DataTypeException);

//    auto type2 = GenH5::DataType::compound(0, {
//        {"type 1", offsetof(Data, a), GenH5::DataType::Int},
//        {"type 2", offsetof(Data, b), GenH5::DataType::Double}
//    });
//    EXPECT_FALSE(type2.isValid());
//    EXPECT_FALSE(type2.isCompound());

    // invalid member
    EXPECT_THROW(GenH5::DataType::compound(0, {
        {"type 1", offsetof(Data, a), GenH5::DataType::Int},
        {"type 2", offsetof(Data, b), GenH5::DataType{}}
    }), GenH5::DataTypeException);

//    auto type3 = GenH5::DataType::compound(sizeof (Data), {
//        {"type 1", offsetof(Data, a), GenH5::DataType::Int},
//        {"type 2", offsetof(Data, b), GenH5::DataType{}}
//    });
//    EXPECT_FALSE(type3.isValid());
//    EXPECT_FALSE(type3.isCompound());

    // invalid member name
    EXPECT_THROW(GenH5::DataType::compound(0, {
        {"type 1", offsetof(Data, a), GenH5::DataType::Int},
        {{}, offsetof(Data, b), GenH5::DataType::Double}
    }), GenH5::DataTypeException);

//    auto type4 = GenH5::DataType::compound(sizeof (Data), {
//        {"type 1", offsetof(Data, a), GenH5::DataType::Int},
//        {{},       offsetof(Data, b), GenH5::DataType::Double}
//    });
//    EXPECT_FALSE(type4.isValid());
//    EXPECT_FALSE(type4.isCompound());
}

TEST_F(TestH5DataType, varLen)
{
    auto doubleType = GenH5::DataType::Double;
    auto vlenType = GenH5::DataType::varLen(doubleType);
    EXPECT_TRUE(vlenType.isValid());
    EXPECT_TRUE(vlenType.isVarLen());
    EXPECT_FALSE(vlenType.isVarString());

    auto vStrType = GenH5::DataType::VarString;
    EXPECT_TRUE(vStrType.isValid());
    EXPECT_TRUE(vStrType.isVarString());
    EXPECT_FALSE(vStrType.isVarLen());

    EXPECT_THROW(GenH5::DataType::varLen(dtypeEmpty), H5::DataTypeIException);
//    auto emptyVlenType = GenH5::DataType::varLen(dtypeEmpty);
//    EXPECT_FALSE(emptyVlenType.isValid());
//    EXPECT_FALSE(emptyVlenType.isVarLen());
}

TEST_F(TestH5DataType, equalToH5cpp)
{
    EXPECT_TRUE(dtypeEmpty == GenH5::DataType{H5::DataType{}});
    EXPECT_TRUE(dtypeInt.toH5()    == H5::PredType::NATIVE_INT);
    EXPECT_TRUE(dtypeDouble.toH5() == H5::PredType::NATIVE_DOUBLE);
    EXPECT_TRUE(dtypeString.toH5() == H5::StrType(H5::PredType::C_S1,
                                                  H5T_VARIABLE));
}
