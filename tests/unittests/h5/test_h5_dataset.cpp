/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5dataset.h"
#include "gt_h5group.h"
#include "gt_h5file.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5DataSet : public testing::Test
{
protected:

    virtual void SetUp()
    {
        doubleData = QVector<double>{1, 2, 3, 4, 5};
        intData    = QVector<int>{1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5", "6"};

        file = GtH5File(TestHelper::instance()->newFilePath(),
                        GtH5File::CreateOverwrite);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());
    }

    GtH5File file;
    GtH5Group group;

    GtH5Data<int> intData;
    GtH5Data<double> doubleData;
    GtH5Data<QString> stringData;
};

TEST_F(TestH5DataSet, isValid)
{
    GtH5DataSet dset;
    // test invalid dataset
    EXPECT_FALSE(dset.isValid());

    // create and test various datasets
    dset = file.root().createDataset(QByteArrayLiteral("test"),
                                     intData.dataType(),
                                     intData.length());
    EXPECT_TRUE(dset.isValid());

    dset = group.createDataset(QByteArrayLiteral("test"),
                               doubleData.dataType(),
                               doubleData.length());
    EXPECT_TRUE(dset.isValid());
}

TEST_F(TestH5DataSet, RW)
{
    // create new dataset
    GtH5DataSet dset = group.createDataset(QByteArrayLiteral("test"),
                                           doubleData.dataType(),
                                           doubleData.length());
    ASSERT_TRUE(dset.isValid());

    // write data
    EXPECT_FALSE(dset.write(Q_NULLPTR));
    EXPECT_TRUE(dset.write(doubleData));

    GtH5Data<double> readData;
    // read data
    EXPECT_FALSE(dset.read(Q_NULLPTR));
    EXPECT_TRUE(dset.read(readData));

    // compare data
    EXPECT_EQ(readData.data(), doubleData.data());
}

TEST_F(TestH5DataSet, deleteLink)
{
    GtH5DataSet dset;
    // test invalid attribute
    EXPECT_FALSE(dset.isValid());

    // create valid attribute
    dset = file.root().createDataset(QByteArrayLiteral("test"),
                                     intData.dataType(),
                                     intData.length());
    EXPECT_TRUE(dset.isValid());

    // delete dataset
    dset.deleteLink();
    EXPECT_FALSE(dset.isValid());
}

TEST_F(TestH5DataSet, resize)
{
    // create new dataset
    GtH5DataSet dset = group.createDataset(QByteArrayLiteral("test"),
                                           doubleData.dataType(),
                                           doubleData.length());
    ASSERT_TRUE(dset.isValid());

    // resize to zero
    EXPECT_TRUE(dset.resize({0}));
    EXPECT_TRUE(dset.isValid());
    EXPECT_EQ(dset.dataSpace().sum(), 0);

    // cannot extend dataset beyond original dimensions
//    EXPECT_FALSE(dset.resize({(uint64_t) (2 * doubleData.length())}));
//    EXPECT_TRUE(dset.isValid());
//    EXPECT_EQ(dset.dataSpace().sum(), 0);
}
