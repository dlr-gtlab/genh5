/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 15.12.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_data.h"

#include "testhelper.h"

#include <QPoint>
#include <QList>
#include <QStringList>

/// This is a test fixture that does a init for each test
struct TestH5FixedString0D : public testing::Test
{
    // conversion by value
    template<typename T>
    void conversion(T value)
    {
        EXPECT_TRUE(true);
        Q_UNUSED(value);
    }
};

TEST_F(TestH5FixedString0D, constructor)
{
    // conversion value
    QByteArray data = "my_fixed_string";
    GenH5::FixedString0D d2{data};
    EXPECT_EQ(d2.raw(), data);
    EXPECT_EQ(d2.value(), data);

    QString data2 = "fancy_String";
    GenH5::FixedString0D d3{data2};
    EXPECT_EQ(d3.raw(), data2.toUtf8());
    EXPECT_EQ(d3.value<std::string>(), data2.toStdString());
    EXPECT_EQ(d3.value<QString>(), data2);

    // test operators
    GenH5::FixedString0D d;
    d = data;
    EXPECT_EQ(d.value(), data);
    d = data2;
    EXPECT_EQ(d.value(), data2);
//    d = "data2";
//    EXPECT_STREQ(d.value(), "data2");
    d = d3;
    EXPECT_EQ(d.value(), d3.value());

    // test converions
    conversion<char*>(d);
    conversion<char const*>(d);
}

TEST_F(TestH5FixedString0D, dataspace)
{
    GenH5::FixedString0D data;
    EXPECT_TRUE(data.dataSpace().isScalar());
}

TEST_F(TestH5FixedString0D, datatype)
{
    GenH5::FixedString0D data("test");
    EXPECT_TRUE(data.dataType().isString());
    EXPECT_FALSE(data.dataType().isVarString());
    EXPECT_EQ(data.dataType().size(), sizeof("test") - 1);
}
