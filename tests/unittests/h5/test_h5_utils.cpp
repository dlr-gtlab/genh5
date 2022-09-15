/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_utils.h"

#include "testhelper.h"

#include <QStringList>

/// This is a test fixture that does a init for each test
class TestH5Utils : public testing::Test
{
protected:
};

TEST_F(TestH5Utils, prod)
{
    // by T out
    auto doubles = { 2.0, 10.1 };
    EXPECT_EQ((GenH5::prod<int>(doubles)), 20);

    EXPECT_EQ((GenH5::prod<int>(QVector<int>{ })), 0);
    EXPECT_EQ((GenH5::prod<int>(QVector<int>{ 12 })), 12);
    EXPECT_EQ((GenH5::prod<int>(QVector<int>{ 42, 1 })), 42);
    EXPECT_EQ((GenH5::prod<int>(QVector<int>{ 42, 0, 2 })), 0);

    EXPECT_EQ((GenH5::prod<float>(std::initializer_list<float>{})), 0.f);

    EXPECT_EQ((GenH5::prod<std::initializer_list<double>, int>(doubles)), 20.);

    // by Container
    EXPECT_EQ((GenH5::prod(QVector<int>{ 42, 1 })), 42);
    EXPECT_EQ((GenH5::prod(QVector<int>{ 42, 0, 2 })), 0);

    EXPECT_EQ((GenH5::prod<std::initializer_list<float>>({})), 0);
}

TEST_F(TestH5Utils, idx)
{
    GenH5::Dimensions dims0{};
    EXPECT_EQ(GenH5::idx(dims0, {}), 0);

    GenH5::Dimensions dims1{42};
    EXPECT_EQ(GenH5::idx(dims1, {12}), 12);

    GenH5::Dimensions dims2{5, 10};
    EXPECT_EQ(GenH5::idx(dims2, {1, 2}), 12);
    EXPECT_EQ(GenH5::idx(dims2, {2, 3}), 23);
    EXPECT_EQ(GenH5::idx(dims2, {4, 9}), 49);

    GenH5::Dimensions dims3{4, 6, 10};
    EXPECT_EQ(GenH5::idx(dims3, {2, 5, 9}), 179);
    EXPECT_EQ(GenH5::idx(dims3, {1, 0, 2}), 62);
    EXPECT_EQ(GenH5::idx(dims3, {0, 0, 0}), 0);

    GenH5::Dimensions dims4{2, 4, 5, 10};
    EXPECT_EQ(GenH5::idx(dims4, {1, 2, 0, 0}), 300);
    EXPECT_EQ(GenH5::idx(dims4, {1, 2, 4, 0}), 340);
    EXPECT_EQ(GenH5::idx(dims4, {1, 3, 4, 9}), 399);

    EXPECT_THROW(GenH5::idx(dims4, {1, 3, 4}), GenH5::InvalidArgumentError);
}

TEST_F(TestH5Utils, reverseComp)
{
    auto tuple = std::make_tuple(42.0f, 12, QString{"hello world"});

    using RT = GenH5::mpl::reversed_comp_t<std::tuple<float, int, QString>>;
    RT rtuple = GenH5::reverseComp(tuple);

    EXPECT_EQ(std::get<0>(tuple), std::get<2>(rtuple));
    EXPECT_EQ(std::get<1>(tuple), std::get<1>(rtuple));
    EXPECT_EQ(std::get<2>(tuple), std::get<0>(rtuple));
}

TEST_F(TestH5Utils, makeArray)
{
    using GenH5::Vector;
    using GenH5::Array;

    // make
    int size = 10;
    Vector<int> ints = h5TestHelper->linearDataVector<int>(size);

    Array<int, 10> array = GenH5::makeArray<10>(ints);

    ASSERT_EQ(array.size(), size);
    ASSERT_TRUE(std::equal(std::cbegin(ints), std::cend(ints),
                           std::cbegin(array)));

    // unpack
    Vector<int> res;
    GenH5::unpack(array, res);

    ASSERT_EQ(res, ints);
}

TEST_F(TestH5Utils, makeArrays)
{
    using GenH5::Vector;
    using GenH5::Array;

    // make
    int N = 10;
    Vector<int> ints1 = h5TestHelper->linearDataVector<int>(N);
    Vector<int> ints2 = h5TestHelper->linearDataVector<int>(N);
    Vector<int> ints3 = h5TestHelper->linearDataVector<int>(N);
    Vector<int> ints4 = h5TestHelper->linearDataVector<int>(N);

    QList<Vector<int>> ints{ints1, ints2, ints3, ints4};

    Vector<Array<int, 10>> arrays = GenH5::makeArrays<10>(ints);

    ASSERT_EQ(arrays.size(), 4);
    ASSERT_EQ(arrays[0].size(), N);

    for (int i = 0; i < 4; ++i)
    {
        EXPECT_TRUE(std::equal(std::cbegin(ints[i]), std::cend(ints[i]),
                               std::cbegin(arrays[i])));
    }

    // unpack
    QList<Vector<int>> res;
    GenH5::unpack(arrays, res);

    EXPECT_EQ(res, ints);
}

TEST_F(TestH5Utils, makeArraysFromList)
{
    using GenH5::Vector;
    using GenH5::Array;

    int N = 11;
    int size = 6;
    Vector<float> floats = h5TestHelper->linearDataVector<float>(N * size);

    Vector<Array<float, 11>> arrays = GenH5::makeArraysFromList<11>(floats);

    ASSERT_EQ(arrays.size(), size);
    ASSERT_EQ(arrays[0].size(), N);

    for (int i = 0; i < size; ++i)
    {
        EXPECT_TRUE(std::equal(std::cbegin(floats) + i * N,
                               std::cbegin(floats) + (i + 1) * N,
                               std::cbegin(arrays[i])));
    }

    // unpack
    QList<Vector<float>> res;
    GenH5::unpack(arrays, res);

    for (int i = 0; i < size; ++i)
    {
        EXPECT_EQ(res[i], floats.mid(i * N, N));
    }
}

TEST_F(TestH5Utils, makeStringArray)
{
    using GenH5::String;
    using GenH5::Vector;
    using GenH5::Array;

    // make string too long
    String stringA = "hello fancy world";
    Array<char, 11> arrayA = GenH5::makeArray<11>(stringA);

    EXPECT_TRUE(std::equal(std::cbegin(arrayA), std::cend(arrayA),
                           std::cbegin(stringA)));

    // unpack
    String resA;
    GenH5::unpack(arrayA, resA);
    EXPECT_EQ(stringA.mid(0, 11), resA);

    // make string too short
    String stringB = "123456789";
    Array<char, 10> arrayB = GenH5::makeArray<10>(stringB);

    EXPECT_TRUE(std::equal(std::cbegin(arrayB), std::cend(arrayB),
                           std::cbegin(stringB + '\0')));

    // unpack
    String resB;
    GenH5::unpack(arrayB, resB);
    EXPECT_EQ(stringB + '\0', resB);
}

TEST_F(TestH5Utils, makeStringArrays)
{
    using GenH5::Vector;
    using GenH5::Array;

    // make
    int N = 10;
    QByteArray str1 = "Hello World";
    QByteArray str2 = "ABCdefGHIjkl";
    QByteArray str3 = "012345678901234";
    QByteArray str4 = "My Test String";

    QByteArrayList strs{str1, str2, str3, str4};

    Vector<Array<char, 10>> arrays = GenH5::makeArrays<10>(strs);

    ASSERT_EQ(arrays.size(), strs.size());
    ASSERT_EQ(arrays[0].size(), N);

    for (int i = 0; i < strs.size(); ++i)
    {
        auto str = strs[i].mid(0, N);
        EXPECT_TRUE(std::equal(std::cbegin(str), std::cend(str),
                               std::cbegin(arrays[i])));
    }

    // unpack
    QList<Vector<char>> res;
    GenH5::unpack(arrays, res);

    EXPECT_EQ(res.size(), arrays.size());
    ASSERT_EQ(res.size(), strs.size());
    for (int i = 0; i < strs.size(); ++i)
    {
        auto str = strs[i].mid(0, N);
        auto& vec = res[i];
        EXPECT_EQ(vec.size(), N);
        EXPECT_EQ(str, QByteArray(vec.constData(), N));
    }
}

TEST_F(TestH5Utils, makeStringArraysFromList)
{
    using GenH5::Vector;
    using GenH5::Array;

    // make
    int N = 11;
    int size = 4;
    QByteArray strs = "Hello World"
                      "ABCdefGHjkl"
                      "01234567890"
                      "My Test Str";

    Vector<Array<char, 11>> arrays = GenH5::makeArraysFromList<11>(strs);

    ASSERT_EQ(arrays.size(), size);
    ASSERT_EQ(arrays[0].size(), N);

    for (int i = 0; i < size; ++i)
    {
        auto str = strs.mid(i * N, N);
        EXPECT_TRUE(std::equal(std::cbegin(str), std::cend(str),
                               std::cbegin(arrays[i])));
    }

    // unpack
    QList<Vector<char>> res;
    GenH5::unpack(arrays, res);

    ASSERT_EQ(res.size(), size);
    EXPECT_EQ(res.size(), arrays.size());
    for (int i = 0; i < size; ++i)
    {
        auto str = strs.mid(i * N, N);
        auto& vec = res[i];
        EXPECT_EQ(vec.size(), N);
        EXPECT_EQ(str, QByteArray(res[i].data(), N));
    }
}

TEST_F(TestH5Utils, makeArraysFromListInvalid)
{
    using GenH5::Vector;
    using GenH5::Array;

    // make
    int N = 11;
    QByteArray strs = "Hello World"
                      "ABCdefGHjkll"
                      "01234567890123"
                      "My Test String";

    ASSERT_NE(strs.size() % N, 0);

    EXPECT_THROW(GenH5::makeArraysFromList<11>(strs),
                 GenH5::InvalidArgumentError);
}

TEST_F(TestH5Utils, makeVarLen)
{
    using GenH5::Vector;
    using GenH5::VarLen;

    // make
    int size = 10;
    Vector<double> doubles = h5TestHelper->linearDataVector<double>(size);

    VarLen<double> varlen = GenH5::makeVarLen(doubles);
    EXPECT_EQ(varlen, doubles);

    // unpack
    std::vector<double> res;
    GenH5::unpack(doubles, res);

    ASSERT_EQ(res.size(), size);
    EXPECT_TRUE(std::equal(std::cbegin(varlen), std::cend(varlen),
                           std::cbegin(res)));
}

TEST_F(TestH5Utils, makeVarLens)
{
    using GenH5::Vector;
    using GenH5::VarLen;

    // make
    int size = 10;
    Vector<double> doubles1 = h5TestHelper->linearDataVector<double>(size);
    Vector<double> doubles2 = h5TestHelper->linearDataVector<double>(size * 2);
    Vector<double> doubles3 = h5TestHelper->linearDataVector<double>(size / 2);
    Vector<double> doubles4 = h5TestHelper->linearDataVector<double>(size / 3);

    QList<Vector<double>> doubles{doubles1, doubles2, doubles3, doubles4};

    Vector<VarLen<double>> varlens = GenH5::makeVarLens(doubles);

    ASSERT_EQ(varlens.size(), doubles.size());

    for (int i = 0; i < doubles.size(); ++i)
    {
        EXPECT_EQ(varlens[i], doubles[i]);
    }

    // unpack
    QList<Vector<double>> res;
    GenH5::unpack(varlens, res);

    EXPECT_EQ(res, doubles);
}

TEST_F(TestH5Utils, makeComp)
{
    using GenH5::Vector;

    int size = 10;
    Vector<double> doubles = h5TestHelper->linearDataVector<double>(size);
    Vector<int> ints = h5TestHelper->linearDataVector<int>(size);
    QStringList strings = h5TestHelper->randomStringList(size);

    auto comp = GenH5::makeComp(doubles, ints, strings);

    ASSERT_EQ(comp.size(), size);

    Vector<double> resD;
    QList<int> resI;
    std::vector<QString> resS;

    GenH5::unpack(comp, resD, resI, resS);

    ASSERT_EQ(resD.size(), size);
    EXPECT_TRUE(std::equal(std::cbegin(resD), std::cend(resD),
                           std::cbegin(doubles)));
    ASSERT_EQ(resI.size(), size);
    EXPECT_TRUE(std::equal(std::cbegin(resI), std::cend(resI),
                           std::cbegin(ints)));
    ASSERT_EQ(resS.size(), size);
    EXPECT_TRUE(std::equal(std::cbegin(resS), std::cend(resS),
                           std::cbegin(strings)));
}

TEST_F(TestH5Utils, makeCompInvalid)
{
    int size = 10;
    auto doubles = h5TestHelper->linearDataVector<double>(size);
    auto ints = h5TestHelper->linearDataVector<int>(size);
    auto strings = h5TestHelper->randomStringList(size * 2);

    EXPECT_THROW(GenH5::makeComp(doubles, ints, strings),
                 GenH5::InvalidArgumentError);
}

TEST_F(TestH5Utils, unpackNested)
{
    using GenH5::Vector;
    using GenH5::Array;
    using GenH5::VarLen;

    int size = 10;
    auto floats1 = h5TestHelper->linearDataVector<float>(size);
    auto floats2 = h5TestHelper->linearDataVector<float>(size*2);

    auto varlens = {
        GenH5::makeVarLen(floats1),
        GenH5::makeVarLen(floats2)
    };

    Array<VarLen<float>, 2> array = GenH5::makeArray<2>(varlens);

    EXPECT_TRUE(std::equal(std::cbegin(floats1), std::cend(floats1),
                           std::cbegin(array[0])));
    EXPECT_TRUE(std::equal(std::cbegin(floats2), std::cend(floats2),
                           std::cbegin(array[1])));

    GenH5::Vector<GenH5::Vector<float>> res;
    GenH5::unpackNested(array, res);

    ASSERT_EQ(res.size(), array.size());
    EXPECT_TRUE(std::equal(std::cbegin(floats1), std::cend(floats1),
                           std::cbegin(res[0])));
    EXPECT_TRUE(std::equal(std::cbegin(floats2), std::cend(floats2),
                           std::cbegin(res[1])));
}
