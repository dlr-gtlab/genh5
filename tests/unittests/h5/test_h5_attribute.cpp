/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_attribute.h"
#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_dataset.h"
#include "genh5_data.h"

#include "testhelper.h"

#include <QStringList>

/// This is a test fixture that does a init for each test
class TestH5Attribute : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        doubleData = GenH5::Vector<double>{1, 2, 3, 4, 5};
        intData    = GenH5::Vector<int>{1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5", "6"};

        file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      intData.dataType(),
                                      GenH5::DataSpace::Scalar);
        ASSERT_TRUE(dataset.isValid());
    }

    GenH5::File file;
    GenH5::Group group;
    GenH5::DataSet dataset;

    GenH5::Data<int> intData;
    GenH5::Data<double> doubleData;
    GenH5::Data<QString> stringData;
};

TEST_F(TestH5Attribute, versionAttr)
{
    EXPECT_TRUE(file.root().createVersionAttribute());

    EXPECT_TRUE(file.root().hasVersionAttribute());

    auto version = file.root().readVersionAttribute();
    auto current = GenH5::Version::current();
    EXPECT_EQ(version.major, current.major);
    EXPECT_EQ(version.minor, current.minor);
    EXPECT_EQ(version.patch, current.patch);
}

TEST_F(TestH5Attribute, isValid)
{
    GenH5::Attribute attr;
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
    GenH5::Attribute attr = group.createAttribute(QByteArrayLiteral("test"),
                                                 doubleData.dataType(),
                                                 doubleData.dataSpace());
    ASSERT_TRUE(attr.isValid());

    // write data
    EXPECT_FALSE(attr.write(nullptr)); // cannot write from null
    EXPECT_TRUE(attr.write(doubleData));

    GenH5::Data<double> readData;
    // read data
    EXPECT_FALSE(attr.read(nullptr)); // cannot read to null
    EXPECT_TRUE(attr.read(readData));

    // compare data
    EXPECT_EQ(readData.c(), doubleData.c());
}

TEST_F(TestH5Attribute, deleteLink)
{
    GenH5::Attribute attr;
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
