/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 19.08.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_data.h"

#include "testhelper.h"

#include <QPoint>
#include <QList>
#include <QStringList>

/// This is a test fixture that does a init for each test
class TestH5Data0D : public testing::Test { };

TEST_F(TestH5Data0D, constructor_sameConversionType)
{
    // conversion value
    GenH5::Data0D<int> d2{15};
    EXPECT_EQ(d2.raw(), 15);
    EXPECT_EQ(d2.value(), 15);

    // test operators
    GenH5::Data0D<int> d;
    d = 42;
    EXPECT_EQ(d.value(), 42);
}

TEST_F(TestH5Data0D, constructor_differentConversionType)
{
    // conversion value
    char val1[] = "MyString";
    GenH5::Data0D<QString> d2{val1};
    EXPECT_STREQ(d2.raw(), val1);

    // template value
    QString str = "Hello World";
    GenH5::Data0D<QString> d5{str};
    EXPECT_EQ(d5.value(), str);

    // test operators
    GenH5::Data0D<QString> d;
    d = val1;
    EXPECT_EQ(d.value(), QString{val1});
    d = str;
    EXPECT_EQ(d.value(), str);
}

TEST_F(TestH5Data0D, array_constructor_sameConversionType)
{
    using GenH5::Array;
    using ArrayT = Array<int, 2>;
    using CArrayT = int[2];

    ArrayT val1{42, 11};
    CArrayT cval1{42, 11};

    // conversion value
    GenH5::Data0D<ArrayT> d2_{val1};
    GenH5::Data0D<CArrayT> d2{val1};
    EXPECT_EQ(d2_.raw(), val1);
    EXPECT_EQ(d2.raw() , val1);

    // template value
    GenH5::Data0D<ArrayT> d5_{cval1};
    GenH5::Data0D<CArrayT> d5{cval1};
    EXPECT_EQ(d5_.value(), val1);
    EXPECT_EQ(d5.value() , val1);

    // test operators
    GenH5::Data0D<ArrayT> d_;
    GenH5::Data0D<CArrayT> d;
    d = val1;
    EXPECT_EQ(d.value(), val1);
    d = cval1;
    EXPECT_EQ(d.value(), val1);
}

TEST_F(TestH5Data0D, array_constructor_differentConversionType)
{
    using GenH5::Array;
    using ArrayT = Array<QString, 2>;
    using CArrayT = QString[2];
    using ConvArrayT = Array<char*, 2>;

    char str2[] = "Hello";
    char str3[] = "!676adö1ä";

    ArrayT val1{str2, str3};
    CArrayT cval1{str2, str3};

    ConvArrayT convVal1{str2, str3};

    // conversion value
    GenH5::Data0D<ArrayT> d2_{convVal1};
    GenH5::Data0D<CArrayT> d2{convVal1};
    EXPECT_EQ(d2_.raw(), convVal1);
    EXPECT_EQ(d2.raw() , convVal1);
    EXPECT_EQ(d2_.value(), val1);
    EXPECT_EQ(d2.value() , val1);

    // template value
    GenH5::Data0D<ArrayT> d5_{cval1};
    GenH5::Data0D<CArrayT> d5{cval1};
    EXPECT_EQ(d5_.value(), val1);
    EXPECT_EQ(d5.value() , val1);

    // test operators
    GenH5::Data0D<ArrayT> d_;
    GenH5::Data0D<CArrayT> d;
    d = val1;
    EXPECT_EQ(d.value(), val1);
    d = cval1;
    EXPECT_EQ(d.value(), val1);
}

TEST_F(TestH5Data0D, varlen_constructor_differentConversionType)
{
    using GenH5::VarLen;
    using VarLenT = VarLen<double>;

    VarLenT val1{42, 11, 53.1};

    hvl_t hvl1{};

    // conversion value
    GenH5::Data0D<VarLenT> d2{hvl1};
    EXPECT_EQ(d2.value(), VarLenT{});

    // template value
    GenH5::Data0D<VarLenT> d5{val1};
    EXPECT_EQ(d5.value(), val1);

    // test operators
    GenH5::Data0D<VarLenT> d;
    d = hvl1;
    EXPECT_EQ(d.value(), VarLenT{});
    d = val1;
    EXPECT_EQ(d.value(), val1);
}

GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);

GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    return GenH5::dataType<T, T>({"xp", "yp"});
};

TEST_F(TestH5Data0D, compound_constructor_differentConversionType)
{
    using GenH5::Comp;
    using CompT = Comp<QString, QPoint>;
    using ConvCompT = Comp<QPoint, char*>;

    char strVal1[] = "Fancy String";
    QString str{strVal1};
    QPoint pt{42, 42};

    CompT val1{strVal1, pt};

    ConvCompT conVal1{pt, strVal1};

    // conversion value
    GenH5::CompData0D<CompT> d2{conVal1};
    EXPECT_EQ(d2.raw(), conVal1);
    EXPECT_EQ(d2.value(), val1);

    // template value
    GenH5::CompData0D<CompT> d5{val1};
    EXPECT_EQ(d5.value(), val1);

    // comp specific: containers in
    GenH5::CompData0D<CompT> d7{str, pt};
    EXPECT_EQ(d7.value(), val1);
    EXPECT_EQ(d7.getValue<0>(), str);
    EXPECT_EQ(d7.getValue<1>(), pt);

    // test operators
    GenH5::Data0D<CompT> d;
    d = conVal1;
    EXPECT_EQ(d.value(), val1);
    d = val1;
    EXPECT_EQ(d.value(), val1);
}

TEST_F(TestH5Data0D, compound_deserialize)
{
    using GenH5::Comp;
    using CompT = Comp<QString, double, char>;

    double dVal1 = 42.0;

    char cVal1 = 'c';

    QString strVal1 = "ABCdef";

    GenH5::CompData0D<CompT> data{strVal1, dVal1, cVal1};

    // deserialize single tuple
    CompT tuple = data.value();
    CompT myTuple = std::make_tuple(strVal1, dVal1, cVal1);
    EXPECT_EQ(tuple, myTuple);

    // deserialize individual tuple element
    EXPECT_EQ((data.getValue<0>()), strVal1);
    EXPECT_EQ((data.getValue<1>()), dVal1);
    EXPECT_EQ((data.getValue<2>()), cVal1);

    // deserialize idx out
    QString strOut;
    double dOut;
    char cOut;
    data.unpack(strOut, dOut, cOut);
    EXPECT_EQ(strOut, strVal1);
    EXPECT_EQ(dOut, dVal1);
    EXPECT_EQ(cOut, cVal1);
}

TEST_F(TestH5Data0D, dataspace)
{
    GenH5::Data0D<QString> data;
    EXPECT_TRUE(data.dataSpace().isScalar());
}
