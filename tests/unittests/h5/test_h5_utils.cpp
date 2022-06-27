/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_utils.h"
#include "genh5_optional.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Utils : public testing::Test
{
protected:
};

TEST_F(TestH5Utils, prod)
{
    auto doubles = { 2.0, 10.1 };
    EXPECT_EQ((GenH5::prod<int>(doubles)), 20);
    EXPECT_EQ((GenH5::prod<int>(QVector<int>{ 42, 1 })), 42);
    EXPECT_EQ((GenH5::prod<int>(QVector<int>{ 42, 0, 2 })), 0);
    EXPECT_EQ((GenH5::prod<float>(std::initializer_list<float>{})), 0);

    EXPECT_EQ((GenH5::prod<std::initializer_list<double>, int>(doubles)), 20);
    EXPECT_EQ((GenH5::prod(QVector<int>{ 42, 1 })), 42);
    EXPECT_EQ((GenH5::prod(QVector<int>{ 42, 0, 2 })), 0);
    EXPECT_EQ((GenH5::prod<std::initializer_list<float>>({})), 0);
}

TEST_F(TestH5Utils, optional)
{
    auto prop1 = GenH5::Optional<int>{};
    EXPECT_TRUE(prop1.isDefault());
    EXPECT_EQ(*prop1, 0);

    auto prop2 = GenH5::Optional<int>{{}};
    EXPECT_FALSE(prop2.isDefault());
    EXPECT_EQ(*prop2, 0);

    auto prop3 = GenH5::Optional<int>{42};
    EXPECT_FALSE(prop3.isDefault());
    EXPECT_EQ(*prop3, 42);

    auto prop4 = prop1;
    EXPECT_TRUE(prop4.isDefault());
    EXPECT_EQ(*prop4, *prop1);

    prop4 = prop3;
    EXPECT_FALSE(prop4.isDefault());
    EXPECT_EQ(*prop4, *prop3);
}

#include <QDebug>

TEST_F(TestH5Utils, makeAndUnpackArray)
{
    int size = 10;
    GenH5::Vector<int> ints = h5TestHelper->linearDataVector<int>(size);

    auto array = GenH5::makeArray<10>(ints);

    ASSERT_EQ(array.size(), size);
    ASSERT_TRUE(std::equal(std::cbegin(ints), std::cend(ints),
                           std::cbegin(array)));

    std::vector<int> res;
    GenH5::unpack(array, res);

    ASSERT_EQ(res.size(), size);
    ASSERT_TRUE(std::equal(std::cbegin(res), std::cend(res),
                           std::cbegin(ints)));
}

TEST_F(TestH5Utils, makeAndUnpackVarLen)
{
    int size = 10;
    GenH5::VarLen<double> doubles = h5TestHelper->linearDataVector<double>(size);

    std::vector<double> res;
    GenH5::unpack(doubles, res);

    ASSERT_EQ(res.size(), size);
    EXPECT_TRUE(std::equal(std::cbegin(doubles), std::cend(doubles),
                           std::cbegin(res)));

    auto varlen = GenH5::makeVarLen(res);

    ASSERT_EQ(varlen.size(), size);
    EXPECT_TRUE(std::equal(std::cbegin(res), std::cend(res),
                           std::cbegin(varlen)));
}

TEST_F(TestH5Utils, makeAndUnpackComp)
{
    int size = 10;
    auto doubles = h5TestHelper->linearDataVector<double>(size);
    auto ints = h5TestHelper->linearDataVector<int>(size);
    auto strings = h5TestHelper->randomStringList(size);

    auto comp = GenH5::makeComp(doubles, ints, strings);

    ASSERT_EQ(comp.size(), size);

    GenH5::Vector<double> resD;
    QList<int> resI;
    QStringList resS;

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

TEST_F(TestH5Utils, makeAndUnpackCompInvalid)
{
    int size = 10;
    auto doubles = h5TestHelper->linearDataVector<double>(size);
    auto ints = h5TestHelper->linearDataVector<int>(size);
    auto strings = h5TestHelper->randomStringList(size * 2);

    EXPECT_THROW(GenH5::makeComp(doubles, ints, strings),
                 GenH5::InvalidArgumentError);
}

TEST_F(TestH5Utils, makeAndUnpackNested)
{
    int size = 10;
    auto floats1 = h5TestHelper->linearDataVector<float>(size);
    auto floats2 = h5TestHelper->linearDataVector<float>(size*2);

    auto array = GenH5::makeArray<2>(
        std::initializer_list<GenH5::VarLen<float>>{
            GenH5::makeVarLen(floats1),
            GenH5::makeVarLen(floats2)
    });

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
