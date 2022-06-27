/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_node.h"
#include "gth5_file.h"
#include "gth5_group.h"
#include "gth5_dataset.h"
#include "gth5_attribute.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Group : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GtH5::File(h5TestHelper->newFilePath(), GtH5::Create);
        ASSERT_TRUE(file.isValid());
    }

    GtH5::File file;
};

TEST_F(TestH5Group, isValid)
{
    GtH5::Group root;
    // test invalid dataset
    EXPECT_FALSE(root.isValid());

    // open the file as a root group
    root = GtH5::Group(file);
    EXPECT_TRUE(root.isValid());

    // create a first sub group
    GtH5::Group group = root.createGroup(QByteArrayLiteral("testA"));
    EXPECT_TRUE(group.isValid());

    // create another nested sub group
    GtH5::Group subGroup = group.createGroup(QByteArrayLiteral("testB"));
    EXPECT_TRUE(subGroup.isValid());
}

TEST_F(TestH5Group, deleteLink)
{
    // create valid attribute
    auto group = file.root().createGroup(QByteArrayLiteral("test"));
    EXPECT_TRUE(group.isValid());
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("test")));

    // delete dataset
    group.deleteLink();
    EXPECT_FALSE(group.isValid());
    EXPECT_FALSE(file.root().exists(QByteArrayLiteral("test")));
}

TEST_F(TestH5Group, openGroupInvalid)
{
    GtH5::Group root;
    // test invalid dataset
    EXPECT_FALSE(root.isValid());

    // open the file as a root group
    root = GtH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GtH5::Group{}.openGroup(QByteArray{"my_fancy_group"}),
                 GtH5::GroupException);
    EXPECT_THROW(root.openGroup(QByteArray{"my_fancy_group"}),
                 GtH5::GroupException);

    EXPECT_TRUE(root.createGroup(QByteArray{"my_fancy_group"}).isValid());

    EXPECT_NO_THROW(root.openGroup(QByteArray{"my_fancy_group"}));
    EXPECT_TRUE(root.openGroup(QByteArray{"my_fancy_group"}).isValid());
}

TEST_F(TestH5Group, openDataSetInvalid)
{
    // open the file as a root group
    GtH5::Group root = GtH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GtH5::Group{}.openDataset(QByteArray{"my_fancy_dset"}),
                 GtH5::DataSetException);
    EXPECT_THROW(root.openDataset(QByteArray{"my_fancy_dset"}),
                 GtH5::DataSetException);

    EXPECT_TRUE(root.createDataset(QByteArray{"my_fancy_dset"},
                                   GtH5::DataType::Int,
                                   GtH5::DataSpace::Scalar).isValid());

    EXPECT_NO_THROW(root.openDataset(QByteArray{"my_fancy_dset"}));
    EXPECT_TRUE(root.openDataset(QByteArray{"my_fancy_dset"}).isValid());
}

TEST_F(TestH5Group, openAttributeInvalid)
{
    // open the file as a root group
    GtH5::Group root = GtH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GtH5::Group{}.openAttribute(QByteArray{"my_fancy_attr"}),
                 GtH5::AttributeException);
    EXPECT_THROW(root.openAttribute(QByteArray{"my_fancy_attr"}),
                 GtH5::AttributeException);

    EXPECT_TRUE(root.createAttribute(QByteArray{"my_fancy_attr"},
                                     GtH5::DataType::Int,
                                     GtH5::DataSpace::Scalar).isValid());

    EXPECT_NO_THROW(root.openAttribute(QByteArray{"my_fancy_attr"}));
    EXPECT_TRUE(root.openAttribute(QByteArray{"my_fancy_attr"}).isValid());
}
