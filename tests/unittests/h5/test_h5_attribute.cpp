/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_attribute.h"
#include "genh5_dataset.h"
#include "genh5_file.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5Attribute : public testing::Test {};

TEST_F(TestH5Attribute, info)
{
    GenH5::Data<int> intData{1, 2, 3, 4, 5};
    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
    auto group = file.root().createGroup("group");
    auto dataset = group.createDataSet("dataset",
                                  intData.dataType(),
                                  intData.dataSpace());

    GenH5::String attrName = "testA";
    dataset.createAttribute(attrName,
                            intData.dataType(),
                            intData.dataSpace());

    EXPECT_TRUE(dataset.hasAttribute(attrName));

    EXPECT_NO_THROW(dataset.attributeInfo(attrName));
    EXPECT_NO_THROW(dataset.attributeInfo(".", attrName));
    EXPECT_NO_THROW(group.attributeInfo("dataset", attrName));
    EXPECT_NO_THROW(file.root().attributeInfo("group/dataset", attrName));

    auto info = dataset.attributeInfo(attrName);
    EXPECT_EQ(info.name, attrName);
    EXPECT_NO_THROW(info.toAttribute(dataset).isValid());

    info = group.attributeInfo("dataset", attrName);
    EXPECT_NO_THROW(info.toAttribute(dataset).isValid());
    EXPECT_NO_THROW(info.toAttribute(group, "dataset").isValid());

    qDebug() << "### EXPECTING ERROR: attribute not found";
    EXPECT_THROW(info.toAttribute(group, "invalid").isValid(),
                 GenH5::AttributeException);
    qDebug() << "### END";
}

TEST_F(TestH5Attribute, deleteLink)
{
    GenH5::Attribute attr;
    // test invalid attribute
    EXPECT_FALSE(attr.isValid());

    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
    // create valid attribute
    attr = file.root().createAttribute("test",
                                       GenH5::dataType<int>(),
                                       GenH5::DataSpace::linear(10));
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(file.root().hasAttribute("test"));

    // delete dataset
    attr.deleteLink();
    EXPECT_FALSE(attr.isValid());
    EXPECT_FALSE(file.root().hasAttribute("test"));
}

TEST_F(TestH5Attribute, deleteLinkNested)
{
    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);

    qDebug() << file.filePath();

    // create nested groups
    auto groupD =
            file.root()
            .createGroup("A")
            .createGroup("B")
            .createGroup("C")
            .createGroup("D");

    EXPECT_TRUE(file.root().exists("A/B/C/D"));

    auto attr = groupD.createAttribute("my_attr",
                                       GenH5::dataType<int>(),
                                       GenH5::DataSpace::linear(10));

    EXPECT_TRUE(groupD.hasAttribute("my_attr"));

    EXPECT_NO_THROW(attr.deleteLink());

    EXPECT_FALSE(groupD.hasAttribute("my_attr"));
}

TEST_F(TestH5Attribute, nodeInfo)
{
    EXPECT_THROW(GenH5::Attribute{}.nodeInfo(), GenH5::AttributeException);

    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
    auto rootAttr = file.root().createAttribute("root_attr",
                                                GenH5::dataType<int>(),
                                                GenH5::DataSpace::Scalar);

    EXPECT_EQ(rootAttr.path(), file.root().name());
    ASSERT_NO_THROW(rootAttr.nodeInfo());
    EXPECT_EQ(rootAttr.nodeInfo().path, file.root().name());
    EXPECT_TRUE(rootAttr.nodeInfo().isGroup());
    EXPECT_TRUE(rootAttr.nodeInfo().toGroup(file.root()).isValid());

    auto dset= file.root().writeDataSet0D("dset", 42);
    auto dsetAttr = dset.createAttribute("dset_attr",
                                         GenH5::dataType<int>(),
                                         GenH5::DataSpace::Scalar);

    EXPECT_EQ(dsetAttr.path(), dset.path());
    ASSERT_NO_THROW(dsetAttr.nodeInfo());
    EXPECT_EQ(dsetAttr.nodeInfo().path, dset.path());
    EXPECT_TRUE(dsetAttr.nodeInfo().isDataSet());
    EXPECT_TRUE(dsetAttr.nodeInfo().toDataSet(file.root()).isValid());
}
