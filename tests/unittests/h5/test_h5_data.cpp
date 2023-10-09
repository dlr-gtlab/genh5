/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 25.08.2021
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
class TestH5Data : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        intData    = {1, 2, 3, 4, 5};
        doubleData = {1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5"};
    }

    GenH5::Vector<int> intData;
    GenH5::Vector<double> doubleData;
    QStringList stringData;

    template<typename T>
    bool testSerialize(GenH5::Vector<T> const& values)
    {
        GenH5::Data<T> data{values};
        EXPECT_EQ(data.raw(), values);
        bool success = (data.raw() == values);

        EXPECT_EQ(data.size(), values.size());
        success &= (values.size() == data.size());

        auto desValues = data.values();
        EXPECT_EQ(desValues, values);
        success &= (desValues == values);

        return success;
    }

    // conversion by value
    template<typename T>
    void conversion(GenH5::Vector<T> value)
    {
        EXPECT_TRUE(true);
        Q_UNUSED(value);
    }
};

/*
    The data classes are supposed to handle many different kinds of types.
    While doing so they need to convert and reverse convert between the original
    type and the conversion type.
    The data classes should have ctors, operators and methods, that are easy to
    use.
    Since the meta programming can get quite messy we test the interface for
    different template types and input data.
*/

// make sure all ctors work as expected
TEST_F(TestH5Data, sameConversionType)
{
    // conversion container
    GenH5::conversion_container_t<int> nativeContainer{1, 2, 3, 4, 5};
    GenH5::Data<int> d1{nativeContainer};
    EXPECT_EQ(d1.raw(), nativeContainer);
    // conversion value
    GenH5::Data<int> d2{15};
    EXPECT_EQ(d2.first(), 15);
    // conversion init list
    GenH5::Data<int> d3{15, 4, 5, 2, 1};

    // other container
    QList<int> list{51, 23, 42};
    GenH5::Data<int> d4{list};
    EXPECT_TRUE(std::equal(std::cbegin(d4), std::cend(d4), std::cbegin(list)));

    // test operators
    GenH5::Data<int> d;
    d = nativeContainer;
    EXPECT_EQ(d.values(), d1.values());
    d = 15;
    EXPECT_EQ(d.values(), d2.values());
    d = list;
    EXPECT_EQ(d.values(), d4.values());

    d.clear();
    ASSERT_EQ(d.size(), 0);

    // test pushback
    d.push_back(nativeContainer);
    EXPECT_EQ(d.values(), d1.values());
    d.push_back(15);
    EXPECT_EQ(d.values(), d1.values() + d2.values());
    d.push_back(list);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values());
    d.push_back(size_t{52});

    // test converions
    conversion<int>(d);
}

TEST_F(TestH5Data, differentConversionType)
{
    char val1[] = "MyString";
    char val2[] = "Hello";
    char val3[] = "T";
    char val4[] = "ABC";
    // conversion container
    GenH5::conversion_container_t<QString> nativeContainer{
        val1, val2, val3, val4
    };
    GenH5::Data<QString> d1{nativeContainer};
    // conversion value
    GenH5::Data<QString> d2{val1};
    // conversion init list
    GenH5::Data<QString> d3{
        std::initializer_list<char*>{val2, val3, val4}
    };

    // other container
    QStringList list{"ABC", "DEF", "Fancy%&!"};
    GenH5::Data<QString> d4{list};
    // template value
    QString str = "Hello World";
    GenH5::Data<QString> d5{str};
    // template init list
    std::initializer_list<QString> initList = {"Hello World", "Test", "Fancy"};
    GenH5::Data<QString> d6{initList};

    // test operators
    GenH5::Data<QString> d;
    d = nativeContainer;
    EXPECT_EQ(d.values(), d1.values());
    d = val1;
    EXPECT_EQ(d.values(), d2.values());
    d = list;
    EXPECT_EQ(d.values(), d4.values());
    d = str;
    EXPECT_EQ(d.values(), d5.values());
    d = initList;
    EXPECT_EQ(d.values(), d6.values());

    d.clear();
    ASSERT_EQ(d.size(), 0);

    // test pushback
    d.push_back(nativeContainer);
    EXPECT_EQ(d.values(), d1.values());
    d.push_back(val1);
    EXPECT_EQ(d.values(), d1.values() + d2.values());
    d.push_back(list);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values());
    d.push_back(str);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values());
    d.push_back(initList);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values() + d6.values());

    // test converions
    conversion<QString>(d);

    /*
     *  test for args with different types compared to CompT
     */

    QStringList list2;
    d4.unpack(list2);
    EXPECT_EQ(list, list2);

    QByteArrayList qbalist;
    d5.unpack(qbalist);
    EXPECT_EQ(qbalist.first(), str);
    EXPECT_EQ(qbalist, d5.values<QByteArrayList>());

    // setters
    QString qba{"Hello World"};
    GenH5::Data<QString> d10{list2};
    EXPECT_NE(d10.value(0), qba);
    d10.setValue(0, qba);
    EXPECT_EQ(d10.value(0), qba);
}

TEST_F(TestH5Data, array_sameConversionType)
{
    using GenH5::Array;
    using ArrayT = Array<int, 2>;
    using CArrayT = int[2]; // test for c-styled arrays

    ArrayT val1{42, 11};
    ArrayT val2{1, 2};
    ArrayT val3{3, 4};
    ArrayT val4{-10, 14};

    CArrayT cval1{42, 11};

    // conversion container
    GenH5::conversion_container_t<CArrayT> nativeContainer{
        val1, val2, val3, val4
    };
    GenH5::Data<ArrayT> d1_{nativeContainer};
    GenH5::Data<CArrayT> d1{nativeContainer};
    // conversion value
    GenH5::Data<ArrayT> d2_{val1};
    GenH5::Data<CArrayT> d2{val1};
    // conversion init list
    GenH5::Data<ArrayT> d3_{val1, val2, val3, val4, {15, 1}};
    GenH5::Data<CArrayT> d3{val1, val2, val3, val4, {15, 1}};

    // other container
    QList<ArrayT> list{val1, val2, val4};
    GenH5::Data<ArrayT> d4_{list};
    GenH5::Data<CArrayT> d4{list};
    // template value
    GenH5::Data<ArrayT> d5_{cval1};
    GenH5::Data<CArrayT> d5{cval1};

    // test operators
    GenH5::Data<CArrayT> d;
    d = nativeContainer;
    EXPECT_EQ(d.values(), d1.values());
    d = val1;
    EXPECT_EQ(d.values(), d2.values());
    d = list;
    EXPECT_EQ(d.values(), d4.values());
    d = cval1;
    EXPECT_EQ(d.values(), d5.values());

    d.clear();
    ASSERT_EQ(d.size(), 0);

    // test pushback
    d.push_back(nativeContainer);
    EXPECT_EQ(d.values(), d1.values());
    d.push_back(val1);
    EXPECT_EQ(d.values(), d1.values() + d2.values());
    d.push_back(list);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values());
    d.push_back(cval1);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values());

    // test converions
    conversion<ArrayT>(d);
}

TEST_F(TestH5Data, array_differentConversionType)
{
    using GenH5::Array;
    using ArrayT = Array<QString, 2>;
    using CArrayT = QString[2]; // test for c-styled arrays
    using ConvArrayT = Array<char*, 2>;

    char convVal1[] = "MyString";
    char convVal2[] = "Hello";
    char convVal3[] = "T";
    char convVal4[] = "!676adö1ä";

    ConvArrayT val1{convVal1, convVal2};
    ConvArrayT val2{convVal2, convVal3};
    ConvArrayT val3{convVal4, convVal1};
    ConvArrayT val4{convVal1, convVal4};

    // conversion container
    GenH5::conversion_container_t<CArrayT> nativeContainer{
        val1, val2, val3, val4
    };
    GenH5::Data<ArrayT> d1_{nativeContainer};
    GenH5::Data<CArrayT> d1{nativeContainer};
    // conversion value
    GenH5::Data<ArrayT> d2_{val1};
    GenH5::Data<CArrayT> d2{val1};
    // conversion init list
    GenH5::Data<ArrayT> d3_{
        std::initializer_list<ConvArrayT>{val1, val2, val3, val4}
    };
    GenH5::Data<CArrayT> d3{
        std::initializer_list<ConvArrayT>{val1, val2, val3, val4}
    };

    // other container
    QList<ArrayT> list{{"ABC", "DEF"}, {"Fancy%&!"}, {"MyString", convVal4}};
    GenH5::Data<ArrayT> d4_{list};
    GenH5::Data<CArrayT> d4{list};
    // template value
    CArrayT cval1{"Hello", convVal4};
    GenH5::Data<ArrayT> d5_{cval1};
    GenH5::Data<CArrayT> d5{cval1};
    // template init list
    std::initializer_list<ArrayT> initList = {{"Hello", convVal4},
                                              {"Test"}, {"Fancy", convVal1}};
    GenH5::Data<ArrayT> d6_{initList};
    GenH5::Data<CArrayT> d6{initList};

    // compound tests
    GenH5::CompData<ArrayT> cd1_{list};
    GenH5::CompData<CArrayT> cd1{list};
    cd1_.push_back(list);
    cd1.push_back(list);
    GenH5::CompData<ArrayT> cd2_{initList};
    GenH5::CompData<CArrayT> cd2{initList};
    cd2_.push_back(initList);
    cd2.push_back(initList);

    // test operators
    GenH5::Data<CArrayT> d;
    d = nativeContainer;
    EXPECT_EQ(d.values(), d1.values());
    d = val1;
    EXPECT_EQ(d.values(), d2.values());
    d = list;
    EXPECT_EQ(d.values(), d4.values());
    d = cval1;
    EXPECT_EQ(d.values(), d5.values());
    d = initList;
    EXPECT_EQ(d.values(), d6.values());

    d.clear();
    ASSERT_EQ(d.size(), 0);

    // test pushback
    d.push_back(nativeContainer);
    EXPECT_EQ(d.values(), d1.values());
    d.push_back(val1);
    EXPECT_EQ(d.values(), d1.values() + d2.values());
    d.push_back(list);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values());
    d.push_back(cval1);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values());
    d.push_back(initList);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values() + d6.values());

    // test converions
    conversion<ArrayT>(d);

    // setters
    using ArrayT2 = Array<QByteArray, 2>;
    ArrayT2 array2{"Hello World", "Test"};
    GenH5::Data<ArrayT> d10{d4};
    EXPECT_NE(d10.value(0)[0], array2[0]);
    EXPECT_NE(d10.value(0)[1], array2[1]);
    d10.setValue(0, array2);
    EXPECT_EQ(d10.value(0)[0], array2[0]);
    EXPECT_EQ(d10.value(0)[1], array2[1]);
}

TEST_F(TestH5Data, varlen_differentConversionType)
{
    using GenH5::VarLen;
    using VarLenT = VarLen<double>;

    VarLenT val1{42, 11, 53.1};
    VarLenT val2{-11.1};
    VarLenT val3{303213, 1218.12, 3.1415, 42};
    VarLenT val4{};

    hvl_t hvl1{};
    hvl_t hvl2{};
    hvl_t hvl3{};

    // conversion container
    GenH5::conversion_container_t<VarLenT> nativeContainer{
        hvl1, hvl2, hvl3
    };
    GenH5::Data<VarLenT> d1{nativeContainer};
    // conversion value
    GenH5::Data<VarLenT> d2{hvl1};
    // conversion init list
    GenH5::Data<VarLenT> d3{std::initializer_list<hvl_t>{hvl1, hvl2, hvl3, {}}};

    // other container
    QList<VarLenT> list{val1, val2, val3, val4};
    GenH5::Data<VarLenT> d4{std::move(list)};
    // template value
    GenH5::Data<VarLenT> d5{val1};
    // template init list
    GenH5::Data<VarLenT> d6{val1, val2, val3, val4};

    // test operators
    GenH5::Data<VarLenT> d;
    d = nativeContainer;
    EXPECT_EQ(d.values(), d1.values());
    d = hvl1;
    EXPECT_EQ(d.values(), d2.values());
    d = list;
    EXPECT_EQ(d.values(), d4.values());
    d = val1;
    EXPECT_EQ(d.values(), d5.values());
    d = {val1, val2, val3, val4};
    EXPECT_EQ(d.values(), d6.values());

    d.clear();
    ASSERT_EQ(d.size(), 0);

    // test pushback
    d.push_back(nativeContainer);
    EXPECT_EQ(d.values(), d1.values());
    d.push_back(hvl1);
    EXPECT_EQ(d.values(), d1.values() + d2.values());
    d.push_back(list);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values());
    d.push_back(val1);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values());

    // test converions
    conversion<VarLenT>(d);
}

GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);

GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    return GenH5::dataType<T, T>({"xp", "yp"});
};

TEST_F(TestH5Data, compound_differentConversionType)
{
    using GenH5::Comp;
    using CompT = Comp<QString, QPoint>;
    using ConvCompT = Comp<QPoint, char*>;

    char strVal1[] = "MyString";
    char strVal2[] = "Hello";
    char strVal3[] = "T";
    char strVal4[] = "!676adö1ä";

    CompT val1{strVal1, {0, 1}};
    CompT val2{strVal2, {2, 1}};
    CompT val3{strVal3, {-12, 23}};
    CompT val4{strVal4, {42, 42}};

    QList<char*> convStrs{strVal1, strVal2, strVal3, strVal4};
    QStringList strsIn{strVal1, strVal2, strVal3, strVal4};
    QList<QPoint> ptsIn{
        std::get<1>(val1), std::get<1>(val2),
        std::get<1>(val3), std::get<1>(val4)
    };

    ConvCompT conVal1{std::get<1>(val1), strVal1};
    ConvCompT conVal2{std::get<1>(val2), strVal2};
    ConvCompT conVal3{std::get<1>(val3), strVal3};
    ConvCompT conVal4{std::get<1>(val4), strVal4};

    // conversion container
    GenH5::conversion_container_t<CompT> nativeContainer{
        conVal1, conVal2, conVal3, conVal4
    };
    GenH5::CompData<CompT> d1{nativeContainer};
    // conversion value
    GenH5::CompData<CompT> d2{conVal1};
    // conversion init list
    GenH5::CompData<CompT> d3{
        std::initializer_list<ConvCompT>{conVal1, conVal2, conVal3, conVal4}
    };

    // other container
    QList<CompT> list{val1, val2, val3, val4};
    GenH5::CompData<CompT> d4{list};
    // template value
    GenH5::CompData<CompT> d5{val1};
    // template init list
    GenH5::CompData<CompT> d6{val1, val2, val3, val4};

    // comp specific: template containers in
    GenH5::CompData<CompT> d7{strsIn, ptsIn};

    // comp specific: conversion containers in
    GenH5::CompData<CompT> d8{convStrs, ptsIn};

    // comp specific: template arg in
    GenH5::CompData<CompT> d9{strsIn.first(), ptsIn.first()};

    // comp specific: conversion args in
    GenH5::CompData<CompT> d10{static_cast<char*>(strVal1), ptsIn.first()};

    // comp specific: other template args in
    GenH5::CompData<CompT> d11{std::string(strVal1), ptsIn.first()};

    GenH5::CompData<QString> cd1{std::string(strVal1)};
    GenH5::CompData<QString> cd2{static_cast<char*>(strVal1)};
    GenH5::CompData<QString> cd3{QString(strVal1)};

    // test pushback
    d10.push_back(nativeContainer);
    d10.push_back(conVal1);
    d10.push_back(list);
    d10.push_back(val1);
    d10.push_back(strsIn.first().toUtf8(), ptsIn.first());

    // test operators
    GenH5::Data<CompT> d;
    d = nativeContainer;
    EXPECT_EQ(d.values(), d1.values());
    d = conVal1;
    EXPECT_EQ(d.values(), d2.values());
    d = list;
    EXPECT_EQ(d.values(), d4.values());
    d = val1;
    EXPECT_EQ(d.values(), d5.values());

    d.clear();
    ASSERT_EQ(d.size(), 0);

    // test pushback
    d.push_back(nativeContainer);
    EXPECT_EQ(d.values(), d1.values());
    d.push_back(conVal1);
    EXPECT_EQ(d.values(), d1.values() + d2.values());
    d.push_back(list);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values());
    d.push_back(val1);
    EXPECT_EQ(d.values(), d1.values() + d2.values() +
              d4.values() + d5.values());

    // test converions
    conversion<CompT>(d);

    // setters
    QByteArray qba{"Test"};
    EXPECT_NE(d10.getValue<0>(0), qba);
    d10.setValue<0>(0, qba);
    EXPECT_EQ(d10.getValue<0>(0), qba);
}

TEST_F(TestH5Data, compound_deserialize)
{
    using GenH5::Comp;
    using CompT = Comp<QString, double, char>;

    double dVal1 = 42.0;
    double dVal2 = -.11;
    double dVal3 = 1623;
    double dVal4 = 19.1;

    char cVal1 = 'c';
    char cVal2 = 'w';
    char cVal3 = '\0';
    char cVal4 = '0';

    char strVal1[] = "ABCdef";
    char strVal2[] = "124589";
    char strVal3[] = "";
    char strVal4[] = "%!ßqys";

    CompT val1{strVal1, dVal1, cVal1};
    CompT val2{strVal2, dVal2, cVal2};
    CompT val3{strVal3, dVal3, cVal3};
    CompT val4{strVal4, dVal4, cVal4};

    QStringList strsIn{strVal1, strVal2, strVal3, strVal4};
    QList<double> dsIn{dVal1, dVal2, dVal3, dVal4};
    QVector<char> charsIn{cVal1, cVal2, cVal3, cVal4};

    GenH5::CompData<CompT> data{strsIn, dsIn, charsIn};

    // deserialize as tuple vector
    GenH5::Vector<CompT> tuples = data.values();
    GenH5::Vector<CompT> myTuples = GenH5::makeComp(strsIn, dsIn, charsIn);
    EXPECT_EQ(tuples, myTuples);

    // deserialize individual tuple vectors
    EXPECT_EQ((data.getValues<0, QStringList>()), strsIn);
    EXPECT_EQ((data.getValues<1, QList<double>>()), dsIn);
    EXPECT_EQ((data.getValues<2, QVector<char>>()), charsIn);

    // deserialize out
    QStringList strsOut;
    QList<double> dsOut;
    QVector<char> csOut;
    data.unpack(strsOut, dsOut, csOut);
    EXPECT_EQ(strsOut, strsIn);
    EXPECT_EQ(dsOut, dsIn);
    EXPECT_EQ(csOut, charsIn);

    // deserialize single tuple
    CompT tuple = data.value(0);
    CompT myTuple = std::make_tuple(strVal1, dVal1, cVal1);
    EXPECT_EQ(tuple, myTuple);

    // deserialize individual tuple element
    EXPECT_EQ((data.getValue<0>(1)), QString{strVal2});
    EXPECT_EQ((data.getValue<1>(1)), dVal2);
    EXPECT_EQ((data.getValue<2>(1)), cVal2);

    // deserialize idx out
    QString strOut;
    double dOut;
    char cOut;
    data.unpack(2, strOut, dOut, cOut);
    EXPECT_EQ(strOut, strVal3);
    EXPECT_EQ(dOut, dVal3);
    EXPECT_EQ(cOut, cVal3);

    /*
     *  test for args with different types compared to CompT
     */

    // cast to QByteArray instead of QString
    EXPECT_EQ((data.getValue<0, QByteArray>(1)), QByteArray{strVal2});
    // use QChar instead of char
    EXPECT_EQ((data.getValue<2, QChar>(1)), QChar{cVal2});

    // cast to QByteArray instead of QString
    auto vals = data.getValues<0, QList<QByteArray>>();
    EXPECT_TRUE(std::equal(std::cbegin(vals), std::cend(vals),
                           std::cbegin(strsIn)));

    // deserialize idx out
    std::string stdstrOut;
    QChar qcOut;
    data.unpack(2, stdstrOut, dOut, qcOut);
    EXPECT_EQ(stdstrOut, strVal3);
    EXPECT_EQ(dOut, dVal3);
    EXPECT_EQ(qcOut, cVal3);

    // deserialize out
    QList<QByteArray> qbasOut;
    QVector<QChar> qcsOut;
    dsOut.clear();
    data.unpack(qbasOut, dsOut, qcsOut);
    EXPECT_EQ(dsOut, dsIn);
    EXPECT_TRUE(std::equal(std::cbegin(qbasOut), std::cend(qbasOut),
                           std::cbegin(strsIn)));
    EXPECT_TRUE(std::equal(std::cbegin(qcsOut), std::cend(qcsOut),
                           std::cbegin(csOut)));
}

TEST_F(TestH5Data, resize_setDimensions)
{
    auto dtype = GenH5::dataType<int>();
    GenH5::Data<int> data;
    ASSERT_EQ(data.size(), 0);

    // resize with null space
    data.resize(GenH5::DataSpace::Null, dtype);
    EXPECT_EQ(data.size(), 0);
    EXPECT_EQ(data.dimensions(), GenH5::Dimensions{});

    data.clear();
    ASSERT_EQ(data.size(), 0);

    // resize with scalar space
    data.resize(GenH5::DataSpace::Scalar, dtype);
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.dimensions(), GenH5::Dimensions{});

    data.clear();
    ASSERT_EQ(data.size(), 0);

    // resize with linear space
    data.resize(GenH5::DataSpace::linear(42), dtype);
    EXPECT_EQ(data.size(), 42);
    EXPECT_EQ(data.dimensions(), GenH5::Dimensions{42});

    data.clear();
    ASSERT_EQ(data.size(), 0);

    // resize with 2D space
    data.resize(GenH5::DataSpace{42, 12}, dtype);
    EXPECT_EQ(data.size(), 42 * 12);
    EXPECT_EQ(data.dimensions(), (GenH5::Dimensions{42, 12}));
}

TEST_F(TestH5Data, resize_array)
{
    constexpr hsize_t spaceLength = 10;
    constexpr hsize_t arrLength = 5;

    // for simple types
    auto space = GenH5::DataSpace::linear(spaceLength);
    auto space2D = GenH5::DataSpace{5, 2};
    auto superType = GenH5::dataType<int>();
    auto arryType = GenH5::dataType<GenH5::Array<int, arrLength>>();

    GenH5::Data<int> data;
    ASSERT_EQ(data.size(), 0);
    // resize using space length
    data.resize(space, superType);
    EXPECT_EQ(data.size(), spaceLength);

    data.clear();
    ASSERT_EQ(data.size(), 0);

    // resize using space * array length
    data.resize(space, arryType);
    EXPECT_EQ(data.size(), spaceLength * arrLength);

    data.clear();
    ASSERT_EQ(data.size(), 0);

    // resize using 2d space * array length
    data.resize(space2D, arryType);
    EXPECT_EQ(data.size(), spaceLength * arrLength);
    EXPECT_EQ(data.dimensions(), (GenH5::Dimensions{5, 2}));
}

TEST_F(TestH5Data, resize_arrayCompound)
{
    constexpr hsize_t spaceLength = 10;
    constexpr hsize_t arrLength = 5;

    // for compound types
    auto space = GenH5::DataSpace::linear(spaceLength);
    auto superType = GenH5::dataType<GenH5::Comp<int>>();
    auto arryType = GenH5::dataType<GenH5::Comp<GenH5::Array<int, arrLength>>>();

    GenH5::Data<int> data;
    ASSERT_EQ(data.size(), 0);
    // resize using space length
    data.resize(space, superType);
    EXPECT_EQ(data.size(), spaceLength);

    data.clear();
    ASSERT_EQ(data.size(), 0);

    // resize using space * array length
    data.resize(space, arryType);
    EXPECT_EQ(data.size(), spaceLength * arrLength);
}

TEST_F(TestH5Data, valueIdx)
{
    GenH5::Dimensions dims{5, 4};
    GenH5::Data<double> data;
    data = h5TestHelper->linearDataVector<double>(GenH5::prod<int>(dims), 0, 1);

    // dataspace to big
    ASSERT_THROW(data.setDimensions({5, 5}), GenH5::InvalidArgumentError);

    ASSERT_NO_THROW(data.setDimensions(dims));
    ASSERT_EQ(data.dataSpace().dimensions(), dims);

    // 1D
    EXPECT_EQ(data[4], 4);
    EXPECT_EQ(data[12], 12);

    EXPECT_EQ(data.at(11), 11);
    EXPECT_EQ(data.at(19), 19);

    EXPECT_EQ(data.value(3), 3);
    EXPECT_EQ(data.value(7), 7);

    // 2D
    EXPECT_EQ(data.at(2, 3), 11);
    EXPECT_EQ(data.at(0, 4), 4);

    EXPECT_EQ(data.value(3, 3), 15);
    EXPECT_EQ(data.value(1, 1), 5);

    // > 2D
    EXPECT_THROW(data.at({1, 2, 3}), GenH5::InvalidArgumentError);
    EXPECT_THROW(data.value({1, 2, 3}), GenH5::InvalidArgumentError);
    EXPECT_THROW((data[{1, 2, 3}]), GenH5::InvalidArgumentError);
}

TEST_F(TestH5Data, compoundValueIdx)
{
    GenH5::Dimensions dims{2, 5, 4};

    int size = GenH5::prod<int>(dims);
    auto ints = h5TestHelper->linearDataVector<int>(size,0, 1);
    auto bas = h5TestHelper->randomByteArrays(size);

    GenH5::Data<int, QByteArray> data{ints, bas};

    ASSERT_NO_THROW(data.setDimensions(dims));
    ASSERT_EQ(data.dataSpace().dimensions(), dims);

    // 1D
    EXPECT_EQ(data.getValue<0>(3), ints[3]);
    EXPECT_EQ(data.getValue<1>(7), bas[7]);

    // 2D
    // data is 3D dataset
    EXPECT_THROW(data.getValue<0>(0, 4), GenH5::InvalidArgumentError);
    EXPECT_THROW(data.getValue<1>(1, 2), GenH5::InvalidArgumentError);

    // 3D
    EXPECT_EQ(data.getValue<0>({1, 2, 3}), ints[1 * 20 + 2 * 4 + 3]);
    EXPECT_EQ(data.getValue<1>({1, 4, 1}), bas[1 * 20 + 4 * 4 + 1]);
}

TEST_F(TestH5Data, compoundValueIdxTest)
{
    GenH5::Dimensions dims{2, 5, 4};

    int size = GenH5::prod<int>(dims);
    auto ints = h5TestHelper->linearDataVector<int>(size,0, 1);
    auto bas = h5TestHelper->randomByteArrays(size);

    GenH5::CompData<QByteArray, int> data{bas, ints};
    data.push_back(bas, ints);
    data.push_back(bas.first(), ints.first());
    data.push_back(bas.first().data(), ints.first());

    ASSERT_NO_THROW(data.setDimensions(dims));
    ASSERT_EQ(data.dataSpace().dimensions(), dims);

    // 1D
    EXPECT_EQ(data.getValue<0>(7), bas[7]);

    // 2D
    // data is 3D dataset
    EXPECT_THROW(data.getValue<0>(0, 4), GenH5::InvalidArgumentError);

    // 3D
    EXPECT_EQ(data.getValue<0>({1, 4, 1}), bas[1 * 20 + 4 * 4 + 1]);
}

TEST_F(TestH5Data, dataspace)
{
    GenH5::Data<QString> data;
    EXPECT_FALSE(data.dataSpace().isScalar());

    data.resize(10);
    EXPECT_EQ(data.dataSpace().size(), 10);
    EXPECT_EQ(data.dataSpace().nDims(), 1);

    data.setDimensions({2, 5});
    EXPECT_EQ(data.dataSpace().size(), 10);
    EXPECT_EQ(data.dataSpace().nDims(), 2);
}

static_assert(sizeof(float) < sizeof(double),
              "A double should be bigger than a float");

TEST_F(TestH5Data, autoresize_smallerType)
{
    GenH5::Data<float> data;

    EXPECT_TRUE(data.resize(GenH5::DataSpace::linear(10),
                            GenH5::dataType<double>()));
    EXPECT_EQ(data.size(), 10);
}

TEST_F(TestH5Data, autoresize_arraySmallerType)
{
    using GenH5::Array;
    GenH5::Data<float> data;

    // Autoresize should not succeed as a float is smaller than a double and
    // we require the same memory footprint
    EXPECT_TRUE(data.resize(GenH5::DataSpace::linear(10),
                            GenH5::dataType<Array<double, 5>>()));
    EXPECT_EQ(data.size(), 10);
}

TEST_F(TestH5Data, autoresize_arraySameType)
{
    using GenH5::Array;
    GenH5::Data<double> data;

    // Autoresize should succeed
    EXPECT_TRUE(data.resize(GenH5::DataSpace::linear(10),
                            GenH5::dataType<Array<double, 5>>()));
    EXPECT_EQ(data.size(), 50);
}

TEST_F(TestH5Data, autoresize_arraySameTypeWithSelection)
{
    using GenH5::Array;
    GenH5::Data<double> data;

    // Autoresize should succeed
    EXPECT_TRUE(data.resize(GenH5::makeSelection(GenH5::DataSpace::linear(100),
                                                 GenH5::Dimensions{10}),
                            GenH5::dataType<Array<double, 5>>()));
    EXPECT_EQ(data.size(), 50);
}

struct PaddedStruct
{
    double a;
    char padding1[4];
    int b;
    char padding2[8];
};

static_assert(sizeof(PaddedStruct) > sizeof(std::tuple<int, double>),
              "The custom struct should be bigger than the tuple");

GENH5_DECLARE_DATATYPE_IMPL(PaddedStruct)
{
    return GenH5::DataType::compound(sizeof(PaddedStruct), {
        {"a", offsetof(PaddedStruct, a), GenH5::dataType<double>()},
        {"b", offsetof(PaddedStruct, b), GenH5::dataType<int>()},
    });
}

TEST_F(TestH5Data, autoresize_compSmallerType)
{
    GenH5::CompData<double, int> data;

    EXPECT_TRUE(data.resize(GenH5::DataSpace::linear(10),
                            GenH5::dataType<PaddedStruct>()));
    EXPECT_EQ(data.size(), 10);
}

TEST_F(TestH5Data, autoresize_compArraySmallerType)
{
    using GenH5::Array;
    GenH5::CompData<double, int> data;

    // Autoresize should not succeed
    EXPECT_TRUE(data.resize(GenH5::DataSpace::linear(10),
                            GenH5::dataType<Array<PaddedStruct, 5>>()));
    EXPECT_EQ(data.size(), 10);
}

TEST_F(TestH5Data, autoresize_compArraySameType)
{
    using GenH5::Array;
    GenH5::Data<PaddedStruct> data;

    // Autoresize should succeed
    EXPECT_TRUE(data.resize(GenH5::DataSpace::linear(10),
                            GenH5::dataType<Array<PaddedStruct, 5>>()));
    EXPECT_EQ(data.size(), 50);
}

TEST_F(TestH5Data, autoresize_compArraySameTypeWithSelection)
{
    using GenH5::Array;
    GenH5::Data<PaddedStruct> data;

    // Autoresize should succeed
    EXPECT_TRUE(data.resize(GenH5::makeSelection(GenH5::DataSpace::linear(100),
                                                 GenH5::Dimensions{10}),
                            GenH5::dataType<Array<PaddedStruct, 5>>()));
    EXPECT_EQ(data.size(), 50);
}

/*
#include "genh5.h"

TEST_F(TestH5Data, autoresize_smallerType2)
{
    using GenH5::Array;

    GenH5::Data<float> data;

    GenH5::File file(h5TestHelper->newFilePath(), GenH5::Create);

    qDebug() << "HERE" << file.filePath();

    auto dset = file.root().createDataSet(
                    "my_name", GenH5::dataType<double>(), GenH5::DataSpace::linear(10));

    ASSERT_TRUE(dset.write(h5TestHelper->linearDataVector(10.0)));

    EXPECT_TRUE(dset.read(data));

    qDebug() << "READ" << data.raw();
}
*/

TEST_F(TestH5Data, buffer_reserve)
{
    using GenH5::details::StaticBuffer;
    using GenH5::Array;
    using GenH5::VarLen;
    using GenH5::Comp;

    int bufferSize = 10;

    // reserving should not be necessary
    StaticBuffer<double> buffer1;
    buffer1.reserve(bufferSize);
    EXPECT_EQ(buffer1().capacity(), 0);

    StaticBuffer<QPoint> buffer2;
    buffer2.reserve(bufferSize);
    EXPECT_EQ(buffer2().capacity(), 0);

    // reserving is necessary
    StaticBuffer<QString> buffer3;
    buffer3.reserve(bufferSize);
    EXPECT_EQ(buffer3().capacity(), bufferSize);

    StaticBuffer<QByteArray> buffer4;
    buffer4.reserve(bufferSize);
    EXPECT_EQ(buffer4().capacity(), bufferSize);

    using CompT1 = Comp<QString, int, QPoint>;
    StaticBuffer<CompT1> buffer5;
    buffer5.reserve(bufferSize);
    EXPECT_EQ(GenH5::rget<0>(buffer5()).capacity(), bufferSize); // = QString
    EXPECT_EQ(GenH5::rget<1>(buffer5()).capacity(), 0);          // = int
    EXPECT_EQ(GenH5::rget<2>(buffer5()).capacity(), 0);          // = QPoint

    StaticBuffer<std::string[5]> buffer6;
    buffer6.reserve(bufferSize);
    EXPECT_EQ(buffer6().capacity(), bufferSize);

    // reserving should not be necessary
    StaticBuffer<Array<float, 5>> buffer7;
    buffer7.reserve(bufferSize);
    EXPECT_EQ(buffer7().capacity(), 0);

    // reserving is necessary
    StaticBuffer<VarLen<double>> buffer8;
    buffer8.reserve(bufferSize);
    EXPECT_EQ(buffer8().capacity(), bufferSize);

    using CompT2 = Comp<VarLen<double>, Array<QString, 42>, QPoint>;
    StaticBuffer<CompT2> buffer9;
    buffer9.reserve(bufferSize);
    EXPECT_EQ(GenH5::rget<0>(buffer9()).capacity(), bufferSize); // = VarLen
    EXPECT_EQ(GenH5::rget<1>(buffer9()).capacity(), bufferSize); // = QString[42]
    EXPECT_EQ(GenH5::rget<2>(buffer9()).capacity(), 0);          // = QPoint
}

TEST_F(TestH5Data, buffer_staticReserve)
{
    using GenH5::details::StaticBuffer;

    int bufferSize = 10;

    {
        StaticBuffer<QString> buffer1;
        buffer1.reserve(10);

        EXPECT_EQ(buffer1().capacity(), bufferSize);

        {
            StaticBuffer<QString> buffer2;
            // should share the same buffer
            ASSERT_EQ(&buffer1(), &buffer2());

            // capacity should not have changed
            buffer2.reserve(bufferSize);
            EXPECT_EQ(buffer2().capacity(), bufferSize);

            buffer2().resize(bufferSize * 0.5);

            {
                StaticBuffer<QString> buffer3;
                ASSERT_EQ(&buffer1(), &buffer3());

                // reserve should now extend the capacity
                buffer2.reserve(bufferSize);
                EXPECT_EQ(buffer2().capacity(), bufferSize * 1.5);
            }

            // capacity should still be the same
            EXPECT_EQ(buffer2().capacity(), bufferSize * 1.5);
        }

        // capacity should still be the same
        EXPECT_EQ(buffer1().capacity(), bufferSize * 1.5);
    }

    // buffer should have cleared
    StaticBuffer<QString> buffer;
    EXPECT_EQ(buffer().capacity(), 0);
}

TEST_F(TestH5Data, buffer_clearing)
{
    using GenH5::details::StaticBuffer;
    using GenH5::Array;
    using GenH5::VarLen;
    using GenH5::Comp;

    int bufferSize = 10;

    // buffer will not be cleared
    StaticBuffer<double> buffer1;
    buffer1().resize(bufferSize);
    buffer1.clear();
    EXPECT_EQ(buffer1().size(), bufferSize);

    StaticBuffer<QPoint> buffer2;
    buffer2().resize(bufferSize);
    buffer2.clear();
    EXPECT_EQ(buffer2().size(), bufferSize);

    // buffer will be cleared
    StaticBuffer<QString> buffer3;
    buffer3().resize(bufferSize);
    buffer3.clear();
    EXPECT_EQ(buffer3().size(), 0);

    StaticBuffer<QByteArray> buffer4;
    buffer4().resize(bufferSize);
    buffer4().clear();
    EXPECT_EQ(buffer4().size(), 0);

    using CompT1 = Comp<QString, int, QPoint>;
    StaticBuffer<CompT1> buffer5;
    GenH5::rget<0>(buffer5()).resize(bufferSize);
    GenH5::rget<1>(buffer5()).resize(bufferSize);
    GenH5::rget<2>(buffer5()).resize(bufferSize);
    buffer5.clear();
    EXPECT_EQ(GenH5::rget<0>(buffer5()).size(), 0);          // = QString
    EXPECT_EQ(GenH5::rget<1>(buffer5()).size(), bufferSize); // = int
    EXPECT_EQ(GenH5::rget<2>(buffer5()).size(), bufferSize); // = QPoint

    StaticBuffer<std::string[5]> buffer6;
    buffer6().resize(bufferSize);
    buffer6.clear();
    EXPECT_EQ(buffer6().size(), 0);

    // buffer will not be cleared
    StaticBuffer<Array<float, 5>> buffer7;
    buffer7().resize(bufferSize);
    buffer7.clear();
    EXPECT_EQ(buffer7().size(), bufferSize);

    // buffer will not be cleared
    StaticBuffer<VarLen<double>> buffer8;
    buffer8().resize(bufferSize);
    buffer8.clear();
    EXPECT_EQ(buffer8().size(), 0);

    using CompT2 = Comp<VarLen<double>, Array<QString, 42>, QPoint>;
    StaticBuffer<CompT2> buffer9;
    GenH5::rget<0>(buffer9()).resize(bufferSize);
    GenH5::rget<1>(buffer9()).resize(bufferSize);
    GenH5::rget<2>(buffer9()).resize(bufferSize);
    buffer9.clear();
    EXPECT_EQ(GenH5::rget<0>(buffer9()).size(), 0);          // = VarLen
    EXPECT_EQ(GenH5::rget<1>(buffer9()).size(), 0);          // = QString[42]
    EXPECT_EQ(GenH5::rget<2>(buffer9()).size(), bufferSize); // = QPoint
}

// test that buffer elements are not invalidated (char* should still be valid)
TEST_F(TestH5Data, buffer_constData)
{
    using GenH5::details::StaticBuffer;

    StaticBuffer<QString> buffer;
    buffer().append(QString{"test"}.toUtf8());

    auto firstA = buffer().first().data();
    auto cdataA = buffer().constData();

    // reallocate buffer
    buffer().resize(1000);

    auto firstB = buffer().first().data();
    auto cdataB = buffer().constData();

    GenH5::convert(QString{"wda"}, buffer);

    // data pointers should have changed
    EXPECT_NE(cdataA, cdataB);
    // but buffer data should still be valid
    EXPECT_EQ(firstA, firstB);
}

#if 0
// using std vectors will invalidate char*
TEST_F(TestH5Data, buffer_constData2)
{
    using GenH5::details::StaticBuffer;

    std::vector<std::string> buffer;
    buffer.push_back("test");


    auto firstA = buffer.front().data();
    auto cdataA = buffer.data();

    // reallocate buffer
    buffer.resize(1000);

    auto firstB = buffer.front().data();
    auto cdataB = buffer.data();

    buffer.push_back("wda");

    // data pointers should have changed
    EXPECT_NE(cdataA, cdataB);
    // but buffer data should still be valid
    EXPECT_EQ(firstA, firstB);
}
#endif
