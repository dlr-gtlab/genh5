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
}

TEST_F(TestH5Group, invalid)
{
    EXPECT_THROW(file.root().createGroup("test/testC"), GenH5::GroupException);
}

TEST_F(TestH5Group, deleteLink)
{
    // create valid attribute
    auto group = file.root().createGroup(QByteArrayLiteral("test"));
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("test")));

    // delete group
    EXPECT_NO_THROW(group.deleteLink());
    EXPECT_FALSE(file.root().exists(QByteArrayLiteral("test")));
}

TEST_F(TestH5Group, deleteLinkNested)
{
    qDebug() << file.filePath();

    // create nested groups
    auto groupD =
            file.root()
            .createGroup("A")
            .createGroup("B")
            .createGroup("C")
            .createGroup("D");

//    auto dset = groupD.writeDataSet0D("dataset", h5TestHelper->linearDataVector(100));
//    EXPECT_TRUE(file.root().exists("A/B/C/D/dataset"));
//    dset.deleteLink();

    EXPECT_TRUE(file.root().exists("A/B/C/D"));

//     delete group D
    EXPECT_NO_THROW(groupD.deleteLink());
    EXPECT_FALSE(file.root().exists("A/B/C/D/"));
    EXPECT_TRUE(file.root().exists("A/B/C/"));

    auto groupA = file.root().openGroup("A");
    EXPECT_NO_THROW(groupA.deleteLink());
    EXPECT_FALSE(file.root().exists("A"));
}

TEST_F(TestH5Group, deleteLinkInvalid)
{
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

    EXPECT_TRUE(root.openGroup("/").isValid());

    EXPECT_THROW(GenH5::Group{}.openGroup("my_fancy_group"),
                 GenH5::GroupException);
    qDebug() << "### EXPECTING ERROR: unable to open group";
    EXPECT_THROW(root.openGroup("my_fancy_group"),
                 GenH5::GroupException);
    qDebug() << "### END";

    EXPECT_TRUE(root.createGroup("my_fancy_group").isValid());

    EXPECT_NO_THROW(root.openGroup("my_fancy_group"));
    EXPECT_TRUE(root.openGroup("my_fancy_group").isValid());
}

TEST_F(TestH5Group, openDataSet)
{
    // open the file as a root group
    GenH5::Group root = GenH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GenH5::Group{}.openDataSet("my_fancy_dset"),
                 GenH5::DataSetException);
    qDebug() << "### EXPECTING ERROR: unable to open dataset";
    EXPECT_THROW(root.openDataSet("my_fancy_dset"),
                 GenH5::DataSetException);
    qDebug() << "### END";

    EXPECT_TRUE(root.createDataSet("my_fancy_dset",
                                   GenH5::DataType::Int,
                                   GenH5::DataSpace::Scalar).isValid());

    EXPECT_NO_THROW(root.openDataSet("my_fancy_dset"));
    EXPECT_TRUE(root.openDataSet("my_fancy_dset").isValid());
}

TEST_F(TestH5Group, openAttribute)
{
    // open the file as a root group
    GenH5::Group root = GenH5::Group(file);
    EXPECT_TRUE(root.isValid());

    EXPECT_THROW(GenH5::Group{}.openAttribute("my_fancy_attr"),
                 GenH5::AttributeException);
    qDebug() << "### EXPECTING ERROR: unable to open attribute";
    EXPECT_THROW(root.openAttribute("my_fancy_attr").isValid(),
                 GenH5::AttributeException);
    qDebug() << "### END";
    qDebug() << "### EXPECTING ERROR: not a datatype";
    EXPECT_THROW(root.createAttribute("my_fancy_attr_2", {}, {}).isValid(),
                 GenH5::AttributeException);
    qDebug() << "### END";

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

    qDebug() << "filepath:" << root.file().filePath();

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

TEST_F(TestH5Group, nodeInfo)
{
    // cannot get info of itself
    EXPECT_THROW(file.root().nodeInfo("."), GenH5::GroupException);

    auto group = file.root().createGroup("test");

    auto dset = group.writeDataSet0D("dset", 42);
    // does not work with leading '/'
    EXPECT_THROW(group.nodeInfo("/dset"), GenH5::GroupException);

    ASSERT_NO_THROW(group.nodeInfo("dset"));
    EXPECT_TRUE(group.nodeInfo("dset").isDataSet());
}
