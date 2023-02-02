/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 19.08.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_data.h"

#include <QPoint>
#include <QList>
#include <QStringList>

/// This is a test fixture that does a init for each test
struct TestH5Data0D : public testing::Test
{
    // conversion by value
    template<typename T>
    void conversion(T value)
    {
        EXPECT_TRUE(true);
        Q_UNUSED(value);
    }
};

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

    // test converions
    conversion<int>(d);
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

    // other conversion type
    EXPECT_EQ(d5.value<QByteArray>(), str.toUtf8());

    // test operators
    GenH5::Data0D<QString> d;
    d = val1;
    EXPECT_STREQ(d.raw(), val1);
    d = str;
    EXPECT_EQ(d.value(), str);

    // test converions
    conversion<QString>(d);
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

    // test converions
    conversion<ArrayT>(d);
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

    // other conversion type
    using ArrayT2 = Array<QByteArray, 2>;
    EXPECT_EQ((d5.value<ArrayT2>()), (ArrayT2{str2, str3}));

    // test operators
    GenH5::Data0D<ArrayT> d_;
    GenH5::Data0D<CArrayT> d;
    d = val1;
    EXPECT_EQ(d.value(), val1);
    d = cval1;
    EXPECT_EQ(d.value(), val1);

    // test converions
    conversion<ArrayT>(d);
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

    // test converions
    conversion<VarLenT>(d);
}

GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);

GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    return GenH5::dataType<T, T>({"xp", "yp"});
};

TEST_F(TestH5Data0D, compound_constructor_sameConversionType)
{
    using GenH5::Comp;
    using CompT = Comp<int, double>;

    // conversion value
    GenH5::CompData0D<CompT> d1{42, 13.1};
    EXPECT_EQ(d1.getValue<0>(), 42);
    EXPECT_EQ(d1.getValue<1>(), 13.1);

    d1.setValue<0>(13);
    d1.setValue<1>(42.1);

    EXPECT_EQ(d1.getValue<0>(), 13);
    EXPECT_EQ(d1.getValue<1>(), 42.1);

    // setters
    GenH5::Data0D<CompT> d;
    d.setValue<0>(13);
    d.setValue<1>(42.1);
    EXPECT_EQ(d1.getValue<0>(), d.getValue<0>());
    EXPECT_EQ(d1.getValue<1>(), d.getValue<1>());

    // test converions
    conversion<CompT>(d);
}

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

    // comp specific: template args in
    GenH5::CompData0D<CompT> d7{str, pt};
    EXPECT_EQ(d7.value(), val1);
    EXPECT_EQ(d7.getValue<0>(), str);
    EXPECT_EQ(d7.getValue<1>(), pt);

    // comp specific: conversion args in
    GenH5::CompData0D<CompT> d8{static_cast<char*>(strVal1), pt};
    EXPECT_EQ(d8.value(), val1);
    EXPECT_EQ(d8.getValue<0>(), str);
    EXPECT_EQ(d8.getValue<1>(), pt);

    // comp specific: other template args in
    GenH5::CompData0D<CompT> d9{std::string(strVal1), pt};
    EXPECT_EQ(d9.value(), val1);
    EXPECT_EQ(d9.getValue<0>(), str);
    EXPECT_EQ(d9.getValue<1>(), pt);

    // other tests
    GenH5::CompData0D<QString> cd1{std::string(strVal1)};
    GenH5::CompData0D<QString> cd2{static_cast<char*>(strVal1)};
    GenH5::CompData0D<QString> cd3{QString(strVal1)};

    // using setters
    d8.setValue<0>(str+str);
    d8.setValue<1>(pt+pt);

    EXPECT_EQ(d8.getValue<0>(), str+str);
    EXPECT_EQ(d8.getValue<1>(), pt+pt);

    d8.setValue<0>(std::string(strVal1));
    d8.setValue<1>(pt);
    EXPECT_EQ(d8.getValue<0>(),QString{strVal1});
    EXPECT_EQ(d8.getValue<1>(), pt);

    static_assert(std::is_same<decltype(d8.getValue<0>()), QString>::value,
                  "return type does not match!");

    // test operators
    GenH5::Data0D<CompT> d;
    d = conVal1;
    EXPECT_EQ(d.value(), val1);
    d = val1;
    EXPECT_EQ(d.value(), val1);

    // test converions
    conversion<CompT>(d);
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

    /*
     *  test for args with different types compared to CompT
     */

    // cast to QByteArray instead of QString
    EXPECT_EQ((data.getValue<0, QByteArray>()), strVal1);
    // use QChar instead of char
    EXPECT_EQ((data.getValue<2, QChar>()), QChar{cVal1});

    // deserialize idx out
    std::string stdstrOut;
    QChar qcOut;
    data.unpack(stdstrOut, dOut, qcOut);
    EXPECT_EQ(stdstrOut, strVal1.toStdString());
    EXPECT_EQ(dOut, dVal1);
    EXPECT_EQ(qcOut, cVal1);
}

TEST_F(TestH5Data0D, dataspace)
{
    GenH5::Data0D<QString> data;
    EXPECT_TRUE(data.dataSpace().isScalar());
}
