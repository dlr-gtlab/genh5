/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5attribute.h"
#include "gt_h5file.h"
#include "gt_h5group.h"
#include "gt_h5dataset.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5Attribute : public testing::Test
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

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      intData.dataType(), 0);
        ASSERT_TRUE(dataset.isValid());
    }

    GtH5File file;
    GtH5Group group;
    GtH5DataSet dataset;

    GtH5Data<int> intData;
    GtH5Data<double> doubleData;
    GtH5Data<QString> stringData;
};


TEST_F(TestH5Attribute, isValid)
{
    GtH5Attribute attr;
    // test invalid attribute
    EXPECT_FALSE(attr.isValid());

    // create and test various attributes
    attr = file.root().createAttribute(QByteArrayLiteral("testA"),
                                       intData.dataType(),
                                       intData.length());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(file.root().hasAttribute(QByteArrayLiteral("testA")));

    attr = group.createAttribute(QByteArrayLiteral("testB"),
                                 doubleData.dataType(),
                                 doubleData.length());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(group.hasAttribute(QByteArrayLiteral("testB")));

    attr = dataset.createAttribute(QByteArrayLiteral("testC"),
                                   stringData.dataType(),
                                   stringData.length());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(dataset.hasAttribute(QByteArrayLiteral("testC")));
}

TEST_F(TestH5Attribute, RW)
{
    // create new dataset
    GtH5Attribute attr = group.createAttribute(QByteArrayLiteral("test"),
                                               doubleData.dataType(),
                                               doubleData.length());
    ASSERT_TRUE(attr.isValid());

    // write data
    EXPECT_FALSE(attr.write(Q_NULLPTR));
    EXPECT_TRUE(attr.write(doubleData));

    GtH5Data<double> readData;
    // read data
    EXPECT_FALSE(attr.read(Q_NULLPTR));
    EXPECT_TRUE(attr.read(readData));

    // compare data
    EXPECT_EQ(readData.data(), doubleData.data());
}

TEST_F(TestH5Attribute, deleteLink)
{
    GtH5Attribute attr;
    // test invalid attribute
    EXPECT_FALSE(attr.isValid());

    // create valid attribute
    attr = dataset.createAttribute(QByteArrayLiteral("test"),
                                   intData.dataType(),
                                   intData.length());
    EXPECT_TRUE(attr.isValid());

    // delete dataset
    attr.deleteLink();
    EXPECT_FALSE(attr.isValid());
}

//TEST_F(TestH5Attribute, accessFlags)
//{
//}
