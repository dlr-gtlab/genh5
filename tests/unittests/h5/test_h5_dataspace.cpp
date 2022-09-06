/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_dataspace.h"


/// This is a test fixture that does a init for each test
class TestH5DataSpace : public testing::Test
{
protected:

    // null
    GenH5::DataSpace dspaceEmpty{};
    // linear
    GenH5::DataSpace dspaceSimple{42};
    // two dim
    GenH5::DataSpace dspaceMulti{ 2, 21 };
};

TEST_F(TestH5DataSpace, isValid)
{
    // a dataspace may have null elements
    EXPECT_TRUE(dspaceEmpty.isValid());
    // a dataspace with any elements is valid
    EXPECT_TRUE(dspaceSimple.isValid());
    EXPECT_TRUE(dspaceMulti.isValid());
    EXPECT_TRUE(GenH5::DataSpace::Null.isValid());
    EXPECT_TRUE(GenH5::DataSpace::Scalar.isValid());
    EXPECT_TRUE(GenH5::DataSpace::linear(42).isValid());
    EXPECT_TRUE((GenH5::DataSpace{12, 5}.isValid()));
}

TEST_F(TestH5DataSpace, size)
{
    EXPECT_EQ(dspaceEmpty.size(), 0);
    EXPECT_EQ(dspaceSimple.size(), 42);
    EXPECT_EQ(dspaceMulti.size(), 42);

    EXPECT_EQ(GenH5::DataSpace::Null.size(), 0);
    EXPECT_EQ(GenH5::DataSpace::Scalar.size(), 1);
    EXPECT_EQ(GenH5::DataSpace::linear(42).size(), 42);
    EXPECT_EQ((GenH5::DataSpace{12, 5}.size()), 60);
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
    EXPECT_EQ(GenH5::DataSpace{}.nDims(), 0);
    EXPECT_EQ(GenH5::DataSpace::Null.nDims(), 0);
    EXPECT_EQ(GenH5::DataSpace::Scalar.nDims(), 0);
    EXPECT_EQ(GenH5::DataSpace::linear(42).nDims(), 1);
    EXPECT_EQ((GenH5::DataSpace{1, 2, 3}.nDims()), 3);

    EXPECT_EQ(dspaceEmpty.nDims(), 0);
    EXPECT_EQ(dspaceSimple.nDims(), 1);
    EXPECT_EQ(dspaceMulti.nDims(), 2);
}

TEST_F(TestH5DataSpace, dimensions)
{
    EXPECT_EQ(GenH5::DataSpace{}.dimensions(), GenH5::Dimensions{});
    EXPECT_EQ(GenH5::DataSpace::Null.dimensions(), GenH5::Dimensions{});
    EXPECT_EQ(GenH5::DataSpace::Scalar.dimensions(), GenH5::Dimensions{});

    auto x = GenH5::DataSpace::Scalar;
    EXPECT_EQ(x.dimensions(), GenH5::Dimensions{});

    EXPECT_EQ(GenH5::DataSpace::linear(42).dimensions(),
              GenH5::Dimensions{42});

    EXPECT_EQ((GenH5::DataSpace{1, 2, 3}.dimensions()),
              (GenH5::Dimensions{1, 2, 3}));
}

TEST_F(TestH5DataSpace, selection)
{
    EXPECT_EQ(GenH5::DataSpace{}.selectionSize(), 0);
    EXPECT_EQ(GenH5::DataSpace::Null.selectionSize(), 0);
    EXPECT_EQ(GenH5::DataSpace::Scalar.selectionSize(), 1);
    EXPECT_EQ(GenH5::DataSpace::linear(1).selectionSize(), 1);
    EXPECT_EQ(GenH5::DataSpace::linear(42).selectionSize(), 42);
    EXPECT_EQ((GenH5::DataSpace{2, 3, 7}.selectionSize()), 42);

    GenH5::DataSpace dspace{2, 3, 7};

    EXPECT_TRUE(GenH5::makeSelection(dspace) == dspace);
    EXPECT_TRUE(GenH5::makeSelection(dspace, dspace.dimensions()) == dspace);
    EXPECT_EQ(GenH5::makeSelection(dspace, {1, 2, 3}).size(), 6);

    // missing nDims will be filled with default values
    EXPECT_NO_THROW(GenH5::makeSelection(dspace, {1, 2}).space());
    EXPECT_NO_THROW(GenH5::makeSelection(dspace, {1, 2, 3}, {1, 2}).space());

    // too many nDims is not allowed
    EXPECT_THROW(GenH5::makeSelection(dspace, {1, 2, 3, 4}).space(),
                 GenH5::DataSpaceException);
}
