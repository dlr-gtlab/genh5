/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 22.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "testhelper.h"

#include "genh5_exception.h"


/// This is a test fixture that does a init for each test
class TestH5Exception : public testing::Test
{
protected:
};

#if 0 // redundant
TEST_F(TestH5Exception, catchGenH5Exception)
{
    char const* msg = "my exception";
    try
    {
        throw GenH5::Exception{msg};
    }
    catch (GenH5::Exception const& e)
    {
        EXPECT_STREQ(msg, e.what());
        return;
    }
    // should not be reached
    ASSERT_FALSE(false);
}

TEST_F(TestH5Exception, catchStdException)
{
    char const* msg = "my exception";
    try
    {
        throw GenH5::Exception{msg};
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(msg, e.what());
        return;
    }
    // should not be reached
    ASSERT_FALSE(false);
}
#endif
