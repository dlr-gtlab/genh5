/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_attribute.h"
#include "gth5_file.h"
#include "gth5_group.h"
#include "gth5_dataset.h"
#include "gth5_data.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5Attribute : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        doubleData = GtH5::Vector<double>{1, 2, 3, 4, 5};
        intData    = GtH5::Vector<int>{1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5", "6"};

        file = GtH5::File(h5TestHelper->newFilePath(), GtH5::Create);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      intData.dataType(),
                                      GtH5::DataSpace::Scalar);
        ASSERT_TRUE(dataset.isValid());
    }

    GtH5::File file;
    GtH5::Group group;
    GtH5::DataSet dataset;

    GtH5::Data<int> intData;
    GtH5::Data<double> doubleData;
    GtH5::Data<QString> stringData;
};


TEST_F(TestH5Attribute, isValid)
{
    GtH5::Attribute attr;
    // test invalid attribute
    EXPECT_FALSE(attr.isValid());

    // create and test various attributes
    attr = file.root().createAttribute(QByteArrayLiteral("testA"),
                                       intData.dataType(),
                                       intData.dataSpace());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(file.root().hasAttribute(QByteArrayLiteral("testA")));

    attr = group.createAttribute(QByteArrayLiteral("testB"),
                                 doubleData.dataType(),
                                 doubleData.dataSpace());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(group.hasAttribute(QByteArrayLiteral("testB")));

    attr = dataset.createAttribute(QByteArrayLiteral("testC"),
                                   stringData.dataType(),
                                   stringData.dataSpace());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(dataset.hasAttribute(QByteArrayLiteral("testC")));
}

TEST_F(TestH5Attribute, RW)
{
    // create new dataset
    GtH5::Attribute attr = group.createAttribute(QByteArrayLiteral("test"),
                                                 doubleData.dataType(),
                                                 doubleData.dataSpace());
    ASSERT_TRUE(attr.isValid());

    // write data
    EXPECT_FALSE(attr.write(nullptr)); // cannot write from null
    EXPECT_TRUE(attr.write(doubleData));

    GtH5::Data<double> readData;
    // read data
    EXPECT_FALSE(attr.read(nullptr)); // cannot read to null
    EXPECT_TRUE(attr.read(readData));

    // compare data
    EXPECT_EQ(readData.c(), doubleData.c());
}

TEST_F(TestH5Attribute, deleteLink)
{
    GtH5::Attribute attr;
    // test invalid attribute
    EXPECT_FALSE(attr.isValid());

    // create valid attribute
    attr = dataset.createAttribute(QByteArrayLiteral("test"),
                                   intData.dataType(),
                                   intData.dataSpace());
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(dataset.hasAttribute(QByteArrayLiteral("test")));

    // delete dataset
    attr.deleteLink();
    EXPECT_FALSE(attr.isValid());
    EXPECT_FALSE(dataset.hasAttribute(QByteArrayLiteral("test")));
}
