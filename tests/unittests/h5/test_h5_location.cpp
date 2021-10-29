/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5file.h"
#include "gt_h5group.h"
#include "gt_h5dataset.h"
#include "gt_h5attribute.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Location : public testing::Test
{
protected:

    virtual void SetUp()
    {
        file = GtH5File(TestHelper::instance()->newFilePath(),
                        GtH5File::CreateOverwrite);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      GtH5Data<int>(), 0);
        ASSERT_TRUE(dataset.isValid());

        attribute = dataset.createAttribute(QByteArrayLiteral("attribute"),
                                            GtH5Data<QString>(), 0);
        ASSERT_TRUE(attribute.isValid());
    }

    GtH5File file;
    GtH5Group group;
    GtH5DataSet dataset;
    GtH5Attribute attribute;
};

TEST_F(TestH5Location, type)
{
    EXPECT_EQ(file.root().type(), GtH5Location::Group);
    EXPECT_EQ(group.type(), GtH5Location::Group);
    EXPECT_EQ(dataset.type(), GtH5Location::DataSet);
    EXPECT_EQ(attribute.type(), GtH5Location::Attribute);
}

TEST_F(TestH5Location, linkName)
{
    EXPECT_EQ(GtH5File().root().name(), QByteArray());
    EXPECT_EQ(GtH5Group().name(), QByteArray());
    EXPECT_EQ(GtH5DataSet().name(), QByteArray());
    EXPECT_EQ(GtH5Attribute().name(), QByteArray());

    EXPECT_EQ(file.root().name(), QByteArrayLiteral("/"));
    EXPECT_EQ(group.name(), QByteArrayLiteral("group"));
    EXPECT_EQ(dataset.name(), QByteArrayLiteral("dataset"));
    EXPECT_EQ(attribute.name(), QByteArrayLiteral("attribute"));
}

TEST_F(TestH5Location, linkPath)
{
    EXPECT_EQ(GtH5File().root().path(), QByteArray());
    EXPECT_EQ(GtH5Group().path(), QByteArray());
    EXPECT_EQ(GtH5DataSet().path(), QByteArray());
    EXPECT_EQ(GtH5Attribute().path(), QByteArray());

    EXPECT_EQ(file.root().path(), QByteArrayLiteral("/"));
    EXPECT_EQ(group.path(), QByteArrayLiteral("/group"));
    EXPECT_EQ(dataset.path(), QByteArrayLiteral("/group/dataset"));

    EXPECT_EQ(attribute.path(), dataset.path());
}

TEST_F(TestH5Location, file)
{
    EXPECT_EQ(GtH5File().root().file(), Q_NULLPTR);
    EXPECT_EQ(GtH5Group().file(), Q_NULLPTR);
    EXPECT_EQ(GtH5DataSet().file(), Q_NULLPTR);
    EXPECT_EQ(GtH5Attribute().file(), Q_NULLPTR);

    EXPECT_EQ(file.root().file(), &file);
    EXPECT_EQ(group.file(), &file);
    EXPECT_EQ(dataset.file(), &file);
    EXPECT_EQ(attribute.file(), &file);
}

TEST_F(TestH5Location, exists)
{
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group/")));
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group/dataset")));
    EXPECT_FALSE(file.root().exists(QByteArrayLiteral("dataset")));
}
