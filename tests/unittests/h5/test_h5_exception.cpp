/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "testhelper.h"

#include "gth5_exception.h"
#include "H5Cpp.h"


/// This is a test fixture that does a init for each test
class TestH5Exception : public testing::Test
{
protected:
};

TEST_F(TestH5Exception, catchGtH5Exception)
{
    char const* msg = "my exception";
    try
    {
        throw GtH5::Exception{msg};
    }
    catch (GtH5::Exception const& e)
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
        throw GtH5::Exception{msg};
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(msg, e.what());
        return;
    }
    // should not be reached
    ASSERT_FALSE(false);
}
