
#include "gtest/gtest.h"
#include "gt_h5object.h"

/// This is a test fixture that does a init for each test
class TestH5 : public testing::Test
{
protected:
};

TEST_F(TestH5, sizeInt)
{
    // the wrapper uses uint64_t and int64_t instead of hsize_t
    // the sizes must be equal!
    EXPECT_EQ(sizeof (uint64_t), sizeof (hsize_t));
    EXPECT_EQ(sizeof (int64_t), sizeof (hsize_t));
}
