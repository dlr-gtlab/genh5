/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_dataset.h"
#include "gth5_file.h"
#include "gth5_group.h"
#include "gth5_data.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5AbstractDataSet : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        emptyData  = QVector<int>();
        intData    = QVector<int>{1, 2, 3, 4, 5, 6};

        doubleData = QVector<double>{1, 2, 3, 4, 5, 6};

        file = GtH5File(TestHelper::instance()->newFilePath(),
                        GtH5File::CreateOverwrite);
        ASSERT_TRUE(file.isValid());
        root = file.root();
        ASSERT_TRUE(root.isValid());

        dataset = root.createDataset(QByteArrayLiteral("test"),
                                     intData.dataType(),
                                     intData.dataSpace());
        ASSERT_TRUE(dataset.isValid());
    }

    GtH5File file;
    GtH5Group root;
    GtH5DataSet dataset;

    GtH5Data<int> emptyData;
    GtH5Data<int> intData;
    GtH5Data<double> doubleData;
};

TEST_F(TestH5AbstractDataSet, validateDTypeAndDSpace)
{
    qDebug() << "here";
    EXPECT_TRUE(dataset.dataSpace() == GtH5DataSpace(intData.length()));
    EXPECT_TRUE(dataset.dataType()  == intData.dataType());

    EXPECT_FALSE(dataset.dataSpace() == GtH5DataSpace(emptyData.length()));
    EXPECT_TRUE(dataset.dataType()   == emptyData.dataType());

    EXPECT_TRUE(dataset.dataSpace() == GtH5DataSpace(doubleData.length()));
    EXPECT_FALSE(dataset.dataType() == doubleData.dataType());
}

TEST_F(TestH5AbstractDataSet, write)
{
    // by void*
    EXPECT_TRUE(dataset.write(intData.dataPtr()));
    // false becoause of nullptr
    EXPECT_FALSE(dataset.write(nullptr));

    // using vector
    EXPECT_TRUE(dataset.write(intData.data()));
    // does not succed because size differs
    EXPECT_FALSE(dataset.write(emptyData.data()));
    // does succed because it does not check for dtype and sizes are equal
    EXPECT_TRUE(dataset.write(doubleData.data()));

    // using data vector
    EXPECT_TRUE(dataset.write(intData));
    // does not succed because size differs
    EXPECT_FALSE(dataset.write(emptyData));
    // does not succed because of dtype mismatch
    EXPECT_FALSE(dataset.write(doubleData));
}

TEST_F(TestH5AbstractDataSet, read)
{
    // by void*
    EXPECT_TRUE(dataset.read(intData.dataPtr()));
    // nullptr not allowed
    EXPECT_FALSE(dataset.read(nullptr));

    // using vector
    EXPECT_TRUE(dataset.read(intData.data()));
    // does succed because vector will be allocated to the correct size
    EXPECT_TRUE(dataset.read(emptyData.data()));
    // does succed because vector will be allocated to the correct size and
    // does not check for dtype mismatch
    EXPECT_TRUE(dataset.read(doubleData.data()));

    // using vector
    EXPECT_TRUE(dataset.read(intData));
    // does succed because vector will be allocated to the correct size
    EXPECT_TRUE(dataset.read(emptyData));
    // does not succed because of dtype mismatch
    EXPECT_FALSE(dataset.read(doubleData));
}

TEST_F(TestH5AbstractDataSet, multiDimensionalReadWrite)
{
    GtH5DataSet dset = root.createDataset(QByteArrayLiteral("multidim_test"),
                                          doubleData.dataType(),
                                          GtH5DataSpace({3, 2}));
    ASSERT_TRUE(dset.isValid());

    // write 1D data as 2D
    EXPECT_TRUE(dset.write(doubleData));

    // read 2D data as 1D
    GtH5Data<double> readData;
    EXPECT_TRUE(dset.read(readData));

    // compare data
    EXPECT_EQ(readData.data(), doubleData.data());

    // compare datatype and dataspace
    EXPECT_TRUE(dset.dataType()  == doubleData.dataType());
    EXPECT_TRUE(dset.dataSpace() == GtH5DataSpace({3, 2}));
    EXPECT_TRUE(dset.dataSpace().sum() == doubleData.length());
}
