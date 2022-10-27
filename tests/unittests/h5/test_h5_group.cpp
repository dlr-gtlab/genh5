/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_node.h"
#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_dataset.h"
#include "genh5_attribute.h"

#include "testhelper.h"

#include <QDebug>

/// This is a test fixture that does a init for each test
class TestH5Group : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
        ASSERT_TRUE(file.isValid());
    }

    GenH5::File file;
};

TEST_F(TestH5Group, isValid)
{
    GenH5::Group root;
    // test invalid dataset
    EXPECT_FALSE(root.isValid());

    // open the file as a root group
    root = GenH5::Group(file);
    EXPECT_TRUE(root.isValid());

    // create a first sub group
    GenH5::Group group = root.createGroup("testA");
    EXPECT_TRUE(group.isValid());

    // create another nested sub group
    GenH5::Group subGroup = group.createGroup("testB");
    EXPECT_TRUE(subGroup.isValid());

    EXPECT_THROW(file.root().createGroup("test/testC"), GenH5::GroupException);
}

TEST_F(TestH5Group, deleteLink)
{
    // create valid attribute
    auto group = file.root().createGroup(QByteArrayLiteral("test"));
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("test")));

    // delete group
    group.deleteLink();
    EXPECT_FALSE(file.root().exists(QByteArrayLiteral("test")));

    // cannot delete root group
    EXPECT_THROW(file.root().deleteLink(), GenH5::LocationException);
}

TEST_F(TestH5Group, openGroup)
{
    GenH5::Group root;
    // test invalid dataset
    EXPECT_FALSE(root.isValid());

    // open the file as a root group
    root = GenH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GenH5::Group{}.openGroup("my_fancy_group"),
                 GenH5::GroupException);
    EXPECT_THROW(root.openGroup("my_fancy_group"),
                 GenH5::GroupException);

    EXPECT_TRUE(root.createGroup("my_fancy_group").isValid());

    EXPECT_NO_THROW(root.openGroup("my_fancy_group"));
    EXPECT_TRUE(root.openGroup("my_fancy_group").isValid());
}

TEST_F(TestH5Group, openDataSet)
{
    // open the file as a root group
    GenH5::Group root = GenH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GenH5::Group{}.openDataset("my_fancy_dset"),
                 GenH5::DataSetException);
    EXPECT_THROW(root.openDataset("my_fancy_dset"),
                 GenH5::DataSetException);

    EXPECT_TRUE(root.createDataset("my_fancy_dset",
                                   GenH5::DataType::Int,
                                   GenH5::DataSpace::Scalar).isValid());

    EXPECT_NO_THROW(root.openDataset("my_fancy_dset"));
    EXPECT_TRUE(root.openDataset("my_fancy_dset").isValid());
}

TEST_F(TestH5Group, openAttribute)
{
    // open the file as a root group
    GenH5::Group root = GenH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GenH5::Group{}.openAttribute("my_fancy_attr"),
                 GenH5::AttributeException);
    EXPECT_THROW(root.openAttribute("my_fancy_attr"),
                 GenH5::AttributeException);

    EXPECT_TRUE(root.createAttribute("my_fancy_attr",
                                     GenH5::DataType::Int,
                                     GenH5::DataSpace::Scalar).isValid());

    EXPECT_NO_THROW(root.openAttribute("my_fancy_attr"));
    EXPECT_TRUE(root.openAttribute("my_fancy_attr").isValid());
}

TEST_F(TestH5Group, writeDataSet)
{
    using GenH5::Data;
    using GenH5::Data0D;
    using GenH5::Comp;
    using GenH5::Array;
    using GenH5::VarLen;
    using GenH5::Vector;

    // open the file as a root group
    auto root = file.root();

    qDebug() << "filepath:" << root.file()->filePath();

    QVector<uint> vec{42u, 12};

    root.writeAttribute("my_attr_1", GenH5::makeCompData(vec));
    root.writeAttribute("my_attr_2", vec);
    root.writeAttribute("my_attr_0d_1", GenH5::makeCompData0D(42u));
    root.writeAttribute0D("my_attr_0d_2", 42ull);
    root.writeAttribute0D("my_attr_0d_comp", Comp<char, int>{'A', 12});
    root.writeAttribute0D("my_attr_0d_array", Array<char, 3>{'A', 'B', 'C'});
    root.writeAttribute0D("my_attr_0d_varlen", VarLen<char>{'A', 'B', 'C'});

    root.writeDataSet("my_dset_1", GenH5::makeCompData(vec));
    root.writeDataSet("my_dset_2", vec);
    root.writeDataSet("my_dset_0d_1", GenH5::makeCompData0D(42u));
    root.writeDataSet0D("my_dset_0d_2", 42ull)
            .writeAttribute0D("test", "test")
            .writeAttribute0D("test2", QString{"test"});
    root.writeDataSet0D("my_dset_0d_comp", Comp<char, int>{'A', 12});
    root.writeDataSet0D("my_dset_0d_array", Array<char, 3>{'A', 'B', 'C'});

    VarLen<char> vlenOrig{'A', 'B', 'C'};
    root.writeDataSet0D("my_dset_0d_varlen", vlenOrig);
    auto vlen = root.readDataSet<VarLen<char>>("my_dset_0d_varlen");
    EXPECT_EQ(vlen.value(0), vlenOrig);
}
