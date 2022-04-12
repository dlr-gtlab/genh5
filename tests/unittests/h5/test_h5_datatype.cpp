/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_datatype.h"
#include "gth5_data.h"


/// This is a test fixture that does a init for each test
class TestH5DataType : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        dtypeInt    = GtH5Data<int>().dataType();
        dtypeDouble = GtH5Data<double>().dataType();
        dtypeString = GtH5Data<QString>().dataType();
    }

    // invalid
    GtH5DataType dtypeEmpty;
    // int
    GtH5DataType dtypeInt;
    // double
    GtH5DataType dtypeDouble;
    // string
    GtH5DataType dtypeString;
};

TEST_F(TestH5DataType, isValid)
{
    EXPECT_FALSE(dtypeEmpty.isValid());

    EXPECT_TRUE(dtypeInt.isValid());
    EXPECT_TRUE(dtypeDouble.isValid());
    EXPECT_TRUE(dtypeString.isValid());
}

TEST_F(TestH5DataType, equal)
{
    EXPECT_FALSE(dtypeInt == dtypeDouble);
    EXPECT_FALSE(dtypeInt == dtypeEmpty);
    EXPECT_FALSE(dtypeDouble == dtypeString);
    EXPECT_FALSE(dtypeDouble == dtypeEmpty);
    EXPECT_FALSE(dtypeString == dtypeInt);
    EXPECT_FALSE(dtypeString == dtypeEmpty);

    // the same objects should be equal
    EXPECT_TRUE(dtypeEmpty == dtypeEmpty);
    EXPECT_TRUE(dtypeInt == dtypeInt);
    EXPECT_TRUE(dtypeDouble == dtypeDouble);
    EXPECT_TRUE(dtypeString == dtypeString);
}

TEST_F(TestH5DataType, hdf5)
{
    EXPECT_TRUE(dtypeEmpty  == GtH5DataType(H5::DataType()));
    EXPECT_TRUE(dtypeInt    == GtH5DataType(H5::PredType::NATIVE_INT));
    EXPECT_TRUE(dtypeDouble == GtH5DataType(H5::PredType::NATIVE_DOUBLE));
    EXPECT_TRUE(dtypeString == GtH5DataType(H5::StrType(H5::PredType::C_S1,
                                                        H5T_VARIABLE)));
}
