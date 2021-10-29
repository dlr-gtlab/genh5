/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5dataspace.h"


/// This is a test fixture that does a init for each test
class TestH5DataSpace : public testing::Test
{
protected:
    virtual void SetUp()
    {
        dspaceEmpty  = GtH5DataSpace();
        dspaceSimple = GtH5DataSpace(42);
        dspaceMulti  = GtH5DataSpace({ 2, 21 });
    }

    // null
    GtH5DataSpace dspaceEmpty;
    // linear
    GtH5DataSpace dspaceSimple;
    // two dim
    GtH5DataSpace dspaceMulti;
};

TEST_F(TestH5DataSpace, isValid)
{
    // a dataspace may have null elements
    EXPECT_TRUE(dspaceEmpty.isValid());
    // a dataspace with any elements is valid
    EXPECT_TRUE(dspaceSimple.isValid());
    EXPECT_TRUE(dspaceMulti.isValid());
}

TEST_F(TestH5DataSpace, sum)
{
    EXPECT_EQ(dspaceEmpty.sum(), 0);
    EXPECT_EQ(dspaceSimple.sum(), 42);
    EXPECT_EQ(dspaceMulti.sum(), 42);

    EXPECT_NE(dspaceEmpty.sum(), 42);
    EXPECT_NE(dspaceSimple.sum(), 0);
    EXPECT_NE(dspaceMulti.sum(), 21);

    // sum with custom vector
    EXPECT_EQ((GtH5DataSpace::sum<double, hsize_t>({ 2.0, 10.1 })), 20);
}

TEST_F(TestH5DataSpace, equal)
{
    // dataspaces do not align
    ASSERT_FALSE(dspaceEmpty  == dspaceSimple);
    ASSERT_FALSE(dspaceEmpty  == dspaceMulti);
    ASSERT_FALSE(dspaceSimple == dspaceMulti);

    // the same objects should be equal
    ASSERT_TRUE(dspaceEmpty  == dspaceEmpty);
    ASSERT_TRUE(dspaceSimple == dspaceSimple);
    ASSERT_TRUE(dspaceMulti  == dspaceMulti);
}

TEST_F(TestH5DataSpace, ndims)
{
    EXPECT_EQ(dspaceEmpty.nDims(), 0);
    EXPECT_EQ(dspaceSimple.nDims(), 1);
    EXPECT_EQ(dspaceMulti.nDims(), 2);

    EXPECT_NE(dspaceMulti.nDims(), 3);
}
