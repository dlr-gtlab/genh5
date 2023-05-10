/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_datatype.h"
#include "genh5_data.h"

#include "H5Cpp.h"

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

TEST_F(TestH5DataType, predefinedTypes)
{
    // all predefined types should be valid
    EXPECT_TRUE(GenH5::DataType::VarString.isValid());
    EXPECT_TRUE(GenH5::DataType::VarString.isString());
    EXPECT_TRUE(GenH5::DataType::VarString.isVarString());

    EXPECT_TRUE(GenH5::DataType::Version.isValid());
    EXPECT_TRUE(GenH5::DataType::Version.isCompound());
    EXPECT_EQ(GenH5::DataType::Version.compoundMembers().size(), 3);

    EXPECT_TRUE(GenH5::DataType::Bool.isValid());
    EXPECT_TRUE(GenH5::DataType::Bool.isInt());
    EXPECT_TRUE(GenH5::DataType::Char.isValid());
    EXPECT_TRUE(GenH5::DataType::Char.isInt());

    EXPECT_TRUE(GenH5::DataType::Int.isValid());
    EXPECT_TRUE(GenH5::DataType::Int.isInt());
    EXPECT_TRUE(GenH5::DataType::Long.isValid());
    EXPECT_TRUE(GenH5::DataType::Long.isInt());
    EXPECT_TRUE(GenH5::DataType::LLong.isValid());
    EXPECT_TRUE(GenH5::DataType::LLong.isInt());
    EXPECT_TRUE(GenH5::DataType::UInt.isValid());
    EXPECT_TRUE(GenH5::DataType::UInt.isInt());
    EXPECT_TRUE(GenH5::DataType::ULong.isValid());
    EXPECT_TRUE(GenH5::DataType::ULong.isInt());
    EXPECT_TRUE(GenH5::DataType::ULLong.isValid());
    EXPECT_TRUE(GenH5::DataType::ULLong.isInt());

    EXPECT_TRUE(GenH5::DataType::Float.isValid());
    EXPECT_TRUE(GenH5::DataType::Float.isFloat());
    EXPECT_TRUE(GenH5::DataType::Double.isValid());
    EXPECT_TRUE(GenH5::DataType::Double.isFloat());
}

TEST_F(TestH5DataType, template_defaultDataTypes) // somewhat redundant
{
    // test simple types
    EXPECT_TRUE(GenH5::dataType<bool>().isValid());

    EXPECT_TRUE(GenH5::dataType<char>().isValid());
    EXPECT_TRUE(GenH5::dataType<char*>().isValid());
    EXPECT_TRUE(GenH5::dataType<char*>().isVarString());
    EXPECT_TRUE(GenH5::dataType<char const*>().isValid());
    EXPECT_TRUE(GenH5::dataType<char const*>().isVarString());

    EXPECT_TRUE(GenH5::dataType<char[5]>().isValid());
    EXPECT_TRUE(GenH5::dataType<char[5]>().isString());
    EXPECT_FALSE(GenH5::dataType<char[5]>().isVarString());

    EXPECT_TRUE(GenH5::dataType<int>().isValid());
    EXPECT_TRUE(GenH5::dataType<int>().isInt());
    EXPECT_TRUE(GenH5::dataType<long int>().isValid());
    EXPECT_TRUE(GenH5::dataType<long int>().isInt());
    EXPECT_TRUE(GenH5::dataType<long long int>().isValid());
    EXPECT_TRUE(GenH5::dataType<long long int>().isInt());
    EXPECT_TRUE(GenH5::dataType<unsigned int>().isValid());
    EXPECT_TRUE(GenH5::dataType<unsigned int>().isInt());
    EXPECT_TRUE(GenH5::dataType<unsigned long int>().isValid());
    EXPECT_TRUE(GenH5::dataType<unsigned long int>().isInt());
    EXPECT_TRUE(GenH5::dataType<unsigned long long int>().isValid());
    EXPECT_TRUE(GenH5::dataType<unsigned long long int>().isInt());

    EXPECT_TRUE(GenH5::dataType<float>().isValid());
    EXPECT_TRUE(GenH5::dataType<float>().isFloat());
    EXPECT_TRUE(GenH5::dataType<double>().isValid());
    EXPECT_TRUE(GenH5::dataType<double>().isFloat());

    EXPECT_TRUE(GenH5::dataType<GenH5::Version>().isValid());
    EXPECT_TRUE(GenH5::dataType<GenH5::Version>().isCompound());
    EXPECT_EQ(GenH5::dataType<GenH5::Version>().compoundMembers().size(), 3);
}

TEST_F(TestH5DataType, template_bindingsDataTypes)
{
    /** STL **/
    EXPECT_TRUE(GenH5::dataType<std::string>().isValid());
    EXPECT_TRUE(GenH5::dataType<std::string>().isVarString());

    /** Qt **/
    EXPECT_TRUE(GenH5::dataType<QByteArray>().isValid());
    EXPECT_TRUE(GenH5::dataType<QByteArray>().isVarString());

    EXPECT_TRUE(GenH5::dataType<QString>().isValid());
    EXPECT_TRUE(GenH5::dataType<QString>().isVarString());
}

// declare datatype implementation for QPoint
GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using Tp = decltype (std::declval<QPoint>().x());
    return GenH5::dataType<Tp, Tp>({"xp", "yp"});
};

TEST_F(TestH5DataType, template_customDataTypes)
{
    auto type = GenH5::dataType<QPoint>();
    ASSERT_TRUE(type.isValid());
    EXPECT_TRUE(type.isCompound());
    EXPECT_EQ(GenH5::dataType<QPoint>().compoundMembers().size(), 2);

    // check if compound members are correct
    for (auto& m: type.compoundMembers())
    {
        EXPECT_TRUE(m.type.isValid());
        EXPECT_FALSE(m.type.isCompound());
        EXPECT_TRUE(m.type == GenH5::dataType<int>());
    }

    EXPECT_TRUE((type.size() == GenH5::dataType<int, int>().size()));
    EXPECT_TRUE((type.type() == GenH5::dataType<int, int>().type()));
    // memory layout is equal to compound type of two ints
    EXPECT_TRUE((GenH5::dataType<QPoint>() == GenH5::dataType<int, int>({"xp", "yp"})));
}

TEST_F(TestH5DataType, invalid)
{
    GenH5::DataType dtype;
    EXPECT_NO_THROW(dtype.compoundMembers());
    EXPECT_NO_THROW(dtype.arrayDimensions());
    EXPECT_NO_THROW(dtype.isVarString());
    EXPECT_NO_THROW(dtype.type());
    EXPECT_NO_THROW(dtype.size());

    EXPECT_THROW(dtype.superType(), GenH5::DataTypeException);
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
                 GenH5::DataTypeException);

    EXPECT_THROW(GenH5::DataType::array(GenH5::DataType::Bool, {}),
                 GenH5::DataTypeException);
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
}

TEST_F(TestH5DataType, varLenInvalid)
{
    EXPECT_THROW(GenH5::DataType::varLen(dtypeEmpty),
                 GenH5::DataTypeException);
}

struct MyData
{
    int a;
    double b;
};

TEST_F(TestH5DataType, compound)
{
    GenH5::CompoundMembers members{
        {"a", offsetof(MyData, a), GenH5::dataType<int>()},
        {"b", offsetof(MyData, b), GenH5::dataType<double>()}
    };

    auto type = GenH5::DataType::compound(sizeof (MyData), members);

    EXPECT_TRUE(type.isValid());
    EXPECT_TRUE(type == (GenH5::dataType<int, double>({"a", "b"})));

    EXPECT_TRUE(type.isCompound());
    EXPECT_EQ(type.size(), sizeof(MyData));

    auto members2 = type.compoundMembers();
    ASSERT_EQ(members2.length(), members.length());
    ASSERT_EQ(members2.length(), 2);
    EXPECT_TRUE(members2.at(0) == members.at(0));
    EXPECT_TRUE(members2.at(1) == members.at(1));
}

TEST_F(TestH5DataType, compoundInvalid)
{
    // empty member list
    auto type1 = GenH5::DataType::compound(sizeof (MyData), {});
    EXPECT_TRUE(type1.isValid());
    EXPECT_TRUE(type1.isCompound());

    // empty data size
    EXPECT_THROW(GenH5::DataType::compound(0, {
        {"type 1", offsetof(MyData, a), GenH5::DataType::Int},
        {"type 2", offsetof(MyData, b), GenH5::DataType::Double}
    }), GenH5::DataTypeException);

    // invalid member
    EXPECT_THROW(GenH5::DataType::compound(sizeof (MyData), {
        {"type 1", offsetof(MyData, a), GenH5::DataType::Int},
        {"type 2", offsetof(MyData, b), GenH5::DataType{}}
    }), GenH5::DataTypeException);

    // invalid member name
    EXPECT_THROW(GenH5::DataType::compound(sizeof (MyData), {
        {"type 1", offsetof(MyData, a), GenH5::DataType::Int},
        {{}, offsetof(MyData, b), GenH5::DataType::Double}
    }), GenH5::DataTypeException);
}

TEST_F(TestH5DataType, getTypeNames)
{
    auto dtype = GenH5::DataType::compound(sizeof (MyData), {
        {"int", offsetof(MyData, a), GenH5::DataType::Int},
        {"double", offsetof(MyData, b), GenH5::DataType::Double}
    });

    auto names = GenH5::getTypeNames<2>(dtype);
    EXPECT_EQ(names[0], "int");
    EXPECT_EQ(names[1], "double");

    // order should be the same as above
    auto dtype2 = GenH5::dataType<int, double>(names);
    auto names2 = GenH5::getTypeNames<2>(dtype2);
    EXPECT_EQ(names2[0], "int");
    EXPECT_EQ(names2[1], "double");

    // not a compound type
    auto dtype3 = GenH5::dataType<double>();
    auto names3 = GenH5::getTypeNames<1>(dtype3);
    EXPECT_TRUE(names3[0].empty());
}

TEST_F(TestH5DataType, template_arrayDataTypes)
{
    using GenH5::dataType;
    using GenH5::Array;
    using GenH5::prod;
    using GenH5::conversion_t;

    using ArrayT1 = Array<QByteArray, 5>;
    EXPECT_TRUE(dataType<ArrayT1>().isValid());
    EXPECT_TRUE(dataType<ArrayT1>().isArray());
    EXPECT_TRUE(dataType<ArrayT1>().superType().isVarString());
    EXPECT_EQ(dataType<ArrayT1>().size(),
              5 * sizeof (conversion_t<QByteArray>));
    EXPECT_EQ(prod(dataType<ArrayT1>().arrayDimensions()), 5);

    // fixed string
    using ArrayT2 = char[10];
    EXPECT_TRUE(dataType<ArrayT2>().isValid());
    EXPECT_TRUE(dataType<ArrayT2>().type() == H5T_STRING);
    EXPECT_FALSE(dataType<ArrayT2>().isVarString());
    EXPECT_FALSE(dataType<ArrayT2>().isArray());
    EXPECT_EQ(dataType<ArrayT2>().size(), 10);
    EXPECT_TRUE(dataType<ArrayT2>().arrayDimensions().empty());

    // array of fixed string
    using ArrayT3 = Array<char[42], 2>;
    EXPECT_TRUE(dataType<ArrayT3>().isValid());
    EXPECT_TRUE(dataType<ArrayT3>().isArray());
    EXPECT_EQ(dataType<ArrayT3>().size(), 2 * 42);

    EXPECT_TRUE(dataType<ArrayT3>().superType().type() == H5T_STRING);
    EXPECT_FALSE(dataType<ArrayT3>().superType().isVarString());
    EXPECT_EQ(dataType<ArrayT3>().superType().size(), 42);

    // carray of doubles
    using ArrayT4 = Array<double, 12>;
    EXPECT_TRUE(dataType<ArrayT4>().isValid());
    EXPECT_TRUE(dataType<ArrayT4>().isArray());
    EXPECT_EQ(dataType<ArrayT4>().size(), 12 * sizeof (double));
    EXPECT_EQ(prod(dataType<ArrayT4>().arrayDimensions()), 12);

    EXPECT_TRUE(dataType<ArrayT4>().superType().type() == H5T_FLOAT);
}

TEST_F(TestH5DataType, template_varlenDataTypes)
{
    using GenH5::dataType;
    using GenH5::VarLen;
    using GenH5::Comp;

    // types
    GenH5::DataType dtype;
    dtype = dataType<VarLen<int>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == dataType<int>());

    dtype = dataType<VarLen<QByteArray>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == dataType<QByteArray>());

    dtype = dataType<GenH5::VarLen<QPoint>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType() == dataType<QPoint>());
}

TEST_F(TestH5DataType, template_compoundDataTypes)
{
    // test some compound types
    GenH5::DataType dtype;
    dtype = GenH5::dataType<GenH5::Comp<QPoint>>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 1);

    dtype = GenH5::dataType<int, QString>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 2);

    dtype = GenH5::dataType<double, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 2);

    dtype = GenH5::dataType<int, char const*>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 2);

    dtype = GenH5::dataType<QByteArray, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 2);

    dtype = GenH5::dataType<double, int, QString>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 3);

    dtype = GenH5::dataType<std::string, double, float>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 3);

    dtype = GenH5::dataType<int, size_t, char const*>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 3);

    dtype = GenH5::dataType<bool, QPoint, QString, size_t>();
    EXPECT_TRUE(dtype.isValid());
    EXPECT_TRUE(dtype.isCompound());
    EXPECT_EQ(dtype.compoundMembers().size(), 4);
}

TEST_F(TestH5DataType, template_complexDataTypes)
{
    using GenH5::dataType;
    using GenH5::VarLen;
    using GenH5::Array;
    using GenH5::Comp;

    // Array
    using CompT = Comp<QPoint, QString>;
    using VarLenT = VarLen<CompT>;
    using ArrayT = Array<VarLenT, 42>;

    // create a complex type and check each member
    auto arrayType = dataType<ArrayT>();
    EXPECT_TRUE(arrayType.isArray());
    EXPECT_EQ(arrayType.size(), 42 * sizeof (hvl_t));

    auto varlenType = dataType<VarLenT>();
    EXPECT_TRUE(varlenType.isVarLen());

    auto compType = dataType<CompT>();
    EXPECT_TRUE(compType.isCompound());

    auto members = compType.compoundMembers();
    EXPECT_EQ(members.size(), 2);
    EXPECT_TRUE(members.at(0).type == GenH5::dataType<QPoint>());
    EXPECT_TRUE(members.at(1).type == GenH5::dataType<QString>());

    EXPECT_TRUE(varlenType.superType() == compType);
    EXPECT_TRUE(arrayType.superType() == varlenType);

    // VarLen
    GenH5::DataType dtype;
    dtype = dataType<VarLen<Comp<double, std::string>>>();
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType().isCompound());
    EXPECT_TRUE(dtype.superType() == (dataType<Comp<double, std::string>>()));

    dtype = dataType<VarLen<Comp<double, int, QPoint>>>();
    EXPECT_TRUE(dtype.isVarLen());
    EXPECT_TRUE(dtype.superType().isCompound());
    EXPECT_TRUE(dtype.superType() == (dataType<Comp<double, int, QPoint>>()));
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

TEST_F(TestH5DataType, equal_2)
{
    using GenH5::dataType;
    using GenH5::VarLen;
    using GenH5::Array;
    using GenH5::Comp;

    // ints
    // does not check signedness
    EXPECT_TRUE(dataType<int>() == dataType<unsigned>());
    EXPECT_TRUE(dataType<int>() != dataType<bool>());
    EXPECT_TRUE(dataType<int>() != dataType<char>());
    EXPECT_TRUE(dataType<char>() == dataType<bool>()); // are int types as well
    bool intEQlong = sizeof(int) == sizeof(long);
    EXPECT_EQ(dataType<int>() == dataType<long>(), intEQlong);
    EXPECT_EQ(dataType<int>() == dataType<unsigned long>(), intEQlong);
    bool intEQllong = sizeof(int) == sizeof(long long);
    EXPECT_EQ(dataType<int>() == dataType<long long>(), intEQllong);
    EXPECT_EQ(dataType<int>() == dataType<unsigned long long>(), intEQllong);

    // floats
    EXPECT_TRUE(dataType<double>() != dataType<float>());

    // strings
    EXPECT_TRUE(dataType<char[5]>() == GenH5::DataType::string(5));
    EXPECT_TRUE((dataType<char[5]>() == dataType<Array<char, 5>>()));
    EXPECT_TRUE((dataType<Array<char, 5>>()).isString());
    EXPECT_TRUE(dataType<char[6]>() != GenH5::DataType::string(5));
    EXPECT_TRUE(dataType<char[6]>() != GenH5::DataType::VarString);
    EXPECT_TRUE(dataType<char*>() == GenH5::DataType::VarString);

    // arrays
    EXPECT_TRUE(dataType<int[42]>() == GenH5::DataType::array(GenH5::DataType::Int, 42));
    EXPECT_TRUE(dataType<int[42]>() != GenH5::DataType::array(GenH5::DataType::Double, 42));
    EXPECT_TRUE(dataType<int[42]>() != GenH5::DataType::array(GenH5::DataType::Int, {24, 2}));

    // varlen
    EXPECT_TRUE(dataType<VarLen<double>>() != dataType<VarLen<float>>());
    EXPECT_TRUE(dataType<VarLen<size_t>>() == dataType<VarLen<size_t>>());

    // compound
    EXPECT_TRUE(dataType<Comp<double>>() == dataType<Comp<double>>());
    EXPECT_TRUE(dataType<Comp<double>>() != dataType<Comp<double>>({"my_type_name"}));
    EXPECT_TRUE(dataType<Comp<double>>({"my_type_name"}) != dataType<Comp<float>>({"my_type_name"}));
    EXPECT_TRUE((dataType<double, int, VarLen<int>>() != dataType<VarLen<int>, int, double>()));
}

TEST_F(TestH5DataType, equalToH5cpp)
{
    EXPECT_TRUE(dtypeInt    == GenH5::DataType{
                    H5::DataType(H5::PredType::NATIVE_INT).getId()});
    EXPECT_TRUE(dtypeDouble == GenH5::DataType{
                    H5::DataType(H5::PredType::NATIVE_DOUBLE).getId()});
    EXPECT_TRUE(dtypeString == GenH5::DataType{
                    H5::StrType(H5::PredType::C_S1, H5T_VARIABLE).getId()});
}
