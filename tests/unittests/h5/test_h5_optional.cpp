/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
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
    // default init
    auto prop1 = GenH5::Optional<int>{};
    EXPECT_TRUE(prop1.isDefault());
    EXPECT_EQ(*prop1, 0);

    // init value
    auto prop2 = GenH5::Optional<int>{{}};
    EXPECT_FALSE(prop2.isDefault());
    EXPECT_EQ(*prop2, 0);

    // init value
    auto prop3 = GenH5::Optional<int>{42};
    EXPECT_FALSE(prop3.isDefault());
    EXPECT_EQ(*prop3, 42);

    auto prop4 = prop1;
    EXPECT_TRUE(prop4.isDefault());
    EXPECT_EQ(*prop4, *prop1);

    prop4 = prop3;
    EXPECT_FALSE(prop4.isDefault());
    EXPECT_EQ(*prop4, *prop3);

    // init with custom default value
    GenH5::Optional<int> prop5{420, true};
    EXPECT_EQ(*prop5, 420);
    EXPECT_TRUE(prop5.isDefault());
    prop5 = 12;
    EXPECT_FALSE(prop5.isDefault());
    EXPECT_EQ(*prop5, 12);

    // no explicit value
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
