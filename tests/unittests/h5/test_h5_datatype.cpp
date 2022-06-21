/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_datatype.h"
#include "gth5_conversion.h"

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
    GtH5::DataType dtypeEmpty{};
    // int
    GtH5::DataType dtypeInt = GtH5::dataType<int>();
    // double
    GtH5::DataType dtypeDouble = GtH5::dataType<double>();
    // string
    GtH5::DataType dtypeString = GtH5::dataType<QString>();
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
    EXPECT_TRUE(GtH5::DataType::VarString.isValid());
    EXPECT_TRUE(GtH5::DataType::Char.isValid());

    EXPECT_TRUE(GtH5::DataType::Int.isValid());
    EXPECT_TRUE(GtH5::DataType::Long.isValid());
    EXPECT_TRUE(GtH5::DataType::LLong.isValid());
    EXPECT_TRUE(GtH5::DataType::UInt.isValid());
    EXPECT_TRUE(GtH5::DataType::ULong.isValid());
    EXPECT_TRUE(GtH5::DataType::ULLong.isValid());

    EXPECT_TRUE(GtH5::DataType::Float.isValid());
    EXPECT_TRUE(GtH5::DataType::Double.isValid());
}

// declare datatype implementation for QPoint
GTH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using Tp = decltype (std::declval<QPoint>().x());
    return GtH5::dataType<Tp, Tp>({"xp", "yp"});
};

TEST_F(TestH5DataType, qpointDtype)
{
    auto type = GtH5::dataType<QPoint>();

    ASSERT_TRUE(type.isValid());
    EXPECT_TRUE(type.isCompound());

    // check if compound members are correct
    EXPECT_EQ(GtH5::dataType<QPoint>().compoundMembers().size(), 2);
    for (auto& m: type.compoundMembers())
    {
        EXPECT_TRUE(m.type.isValid());
        EXPECT_FALSE(m.type.isCompound());
        EXPECT_TRUE(m.type == GtH5::dataType<int>());
    }


    EXPECT_TRUE((type.size() == GtH5::dataType<int, int>().size()));
    EXPECT_TRUE((type.type() == GtH5::dataType<int, int>().type()));
    // memory layout is equal to compound type of two ints
    EXPECT_TRUE((GtH5::dataType<QPoint>() == GtH5::dataType<int, int>()));
}

TEST_F(TestH5DataType, fromTemplateSimple)
{
    // test simple types
    EXPECT_TRUE(GtH5::dataType<int>().isValid());
    EXPECT_TRUE(GtH5::dataType<long>().isValid());
    EXPECT_TRUE(GtH5::dataType<float>().isValid());
    EXPECT_TRUE(GtH5::dataType<double>().isValid());
    EXPECT_TRUE(GtH5::dataType<char const*>().isValid());
    EXPECT_TRUE(GtH5::dataType<char*>().isValid());
    EXPECT_TRUE(GtH5::dataType<char>().isValid());
    EXPECT_TRUE(GtH5::dataType<QString>().isValid());
    EXPECT_TRUE(GtH5::dataType<QByteArray>().isValid());

    EXPECT_TRUE(GtH5::dataType<QPoint>().isValid());
}

TEST_F(TestH5DataType, fromTemplateCompound)
{
    // test some compound types
    GtH5::DataType dtype;
    dtype = GtH5::dataType<int, QString>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<double, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<int, char const*>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<QByteArray, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<double, int, QString>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<QString, double, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<int, size_t, char const*>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    dtype = GtH5::dataType<char, QPoint>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());

    /* WILL PROCDUCE BUILD ERRORS */
    // contains unsupported datatypes
//    EXPECT_FALSE(GtH5::dataType<bool>().isValid());
//    EXPECT_FALSE(GtH5::dataType<QPointF>().isValid());
//    EXPECT_FALSE((GtH5::dataType<double, bool>().isValid()));
//    EXPECT_FALSE((GtH5::dataType<int, QPointF, QString>().isValid()));
//    EXPECT_FALSE((GtH5::dataType<QPointF, bool, char>().isValid()));
}

TEST_F(TestH5DataType, fromTemplateVarLen)
{
    // types
    GtH5::DataType dtype;
    dtype = GtH5::dataType<GtH5::VarLen<int>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == GtH5::dataType<int>());

    dtype = GtH5::dataType<GtH5::VarLen<QByteArray>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == GtH5::dataType<QByteArray>());

    dtype = GtH5::dataType<GtH5::VarLen<QPoint>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == GtH5::dataType<QPoint>());

    // compound types
    dtype = GtH5::dataType<GtH5::VarLen<GtH5::Comp<double, QString>>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE((dtype.superType() ==
                 GtH5::dataType<GtH5::Comp<double, QString>>()));

    dtype = GtH5::dataType<GtH5::VarLen<GtH5::Comp<double, int, QPoint>>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE((dtype.superType() ==
                 GtH5::dataType<GtH5::Comp<double, int, QPoint>>()));
}

#include "gth5_data.h"

TEST_F(TestH5DataType, fromTemplateComplex)
{
    using CompT = GtH5::Comp<QPoint, QString>;
    using VarLenT = GtH5::VarLen<CompT>;
    using ArrayT = GtH5::Array<VarLenT, 42>;

    // create a complex type and check each member
    auto arrayType = GtH5::dataType<ArrayT>();
    EXPECT_TRUE(arrayType.isValid());
    EXPECT_TRUE(arrayType.isArray());
    EXPECT_TRUE(arrayType.arrayDimensions() == GtH5::Dimensions{42});

    auto varlenType =  GtH5::dataType<VarLenT>();
    EXPECT_TRUE(varlenType.isValid());
    EXPECT_TRUE(varlenType.isVarLen());

    auto compType =  GtH5::dataType<CompT>();
    EXPECT_TRUE(compType.isValid());
    EXPECT_TRUE(compType.isCompound());

    auto members = compType.compoundMembers();
    EXPECT_EQ(members.size(), 2);
    EXPECT_TRUE(members.at(0).type == GtH5::dataType<QPoint>());
    EXPECT_TRUE(members.at(1).type == GtH5::dataType<QString>());

    EXPECT_TRUE(varlenType.superType() == compType);

    EXPECT_TRUE(arrayType.superType() == varlenType);
}

TEST_F(TestH5DataType, array)
{
    GtH5::Dimensions dims{10, 2};
    auto arrayType = GtH5::DataType::array(GtH5::DataType::Char, dims);
    EXPECT_TRUE(arrayType.isValid());

    EXPECT_TRUE(arrayType.isArray());
    EXPECT_EQ(arrayType.arrayDimensions(), dims);
}

TEST_F(TestH5DataType, arrayInvalid)
{
    GtH5::Dimensions dims{10, 2};
    auto arrayType = GtH5::DataType::array(GtH5::DataType{}, dims);
    EXPECT_FALSE(arrayType.isValid());

    EXPECT_FALSE(arrayType.isArray());
    EXPECT_EQ(arrayType.arrayDimensions(), GtH5::Dimensions{});
}

struct Data
{
    int a;
    double b;
};

TEST_F(TestH5DataType, compound)
{
    GtH5::CompoundMembers members{
        {"a", offsetof(Data, a), GtH5::dataType<int>()},
        {"b", offsetof(Data, b), GtH5::dataType<double>()}
    };

    auto type = GtH5::DataType::compound(sizeof (Data), members);

    EXPECT_TRUE(type.isValid());
    EXPECT_TRUE(type == (GtH5::dataType<int, double>()));

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
    auto type1 = GtH5::DataType::compound(sizeof (Data), {});
    EXPECT_FALSE(type1.isValid());
    EXPECT_FALSE(type1.isCompound());

    // empty data size
    auto type2 = GtH5::DataType::compound(0, {
        {"type 1", offsetof(Data, a), GtH5::DataType::Int},
        {"type 2", offsetof(Data, b), GtH5::DataType::Double}
    });
    EXPECT_FALSE(type2.isValid());
    EXPECT_FALSE(type2.isCompound());

    // invalid member
    auto type3 = GtH5::DataType::compound(sizeof (Data), {
        {"type 1", offsetof(Data, a), GtH5::DataType::Int},
        {"type 2", offsetof(Data, b), GtH5::DataType{}}
    });
    EXPECT_FALSE(type3.isValid());
    EXPECT_FALSE(type3.isCompound());

    // invalid member name
    auto type4 = GtH5::DataType::compound(sizeof (Data), {
        {"type 1", offsetof(Data, a), GtH5::DataType::Int},
        {{},       offsetof(Data, b), GtH5::DataType::Double}
    });
    EXPECT_FALSE(type4.isValid());
    EXPECT_FALSE(type4.isCompound());
}

TEST_F(TestH5DataType, varLen)
{
    auto doubleType = GtH5::DataType::Double;
    auto vlenType = GtH5::DataType::varLen(doubleType);
    EXPECT_TRUE(vlenType.isValid());
    EXPECT_TRUE(vlenType.isVarLen());
    EXPECT_FALSE(vlenType.isVarString());

    auto vStrType = GtH5::DataType::VarString;
    EXPECT_TRUE(vStrType.isValid());
    EXPECT_TRUE(vStrType.isVarString());
    EXPECT_FALSE(vStrType.isVarLen());

    auto emptyVlenType = GtH5::DataType::varLen(dtypeEmpty);
    EXPECT_FALSE(emptyVlenType.isValid());
    EXPECT_FALSE(emptyVlenType.isVarLen());
}

TEST_F(TestH5DataType, equalToH5cpp)
{
    EXPECT_TRUE(dtypeEmpty == GtH5::DataType{H5::DataType{}});
    EXPECT_TRUE(dtypeInt.toH5()    == H5::PredType::NATIVE_INT);
    EXPECT_TRUE(dtypeDouble.toH5() == H5::PredType::NATIVE_DOUBLE);
    EXPECT_TRUE(dtypeString.toH5() == H5::StrType(H5::PredType::C_S1,
                                                  H5T_VARIABLE));
}
