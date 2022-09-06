/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 12.08.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_optional.h"

/// This is a test fixture that does a init for each test
class TestH5Optional : public testing::Test
{
protected:
};

TEST_F(TestH5Optional, optional)
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

    GenH5::Optional<int> prop5;
    EXPECT_EQ(*prop5, 0);
    EXPECT_TRUE(prop5.isDefault());
    prop5 = 12;
    EXPECT_FALSE(prop5.isDefault());
    EXPECT_EQ(*prop5, 12);

    GenH5::Optional<int> prop6;
    EXPECT_EQ(*prop6, 0);
    EXPECT_TRUE(prop6.isDefault());
    prop6 = prop3;
    EXPECT_FALSE(prop5.isDefault());
    EXPECT_EQ(*prop6, *prop3);

    prop6.clear();
    EXPECT_EQ(*prop6, 0);
    EXPECT_TRUE(prop6.isDefault());
}
