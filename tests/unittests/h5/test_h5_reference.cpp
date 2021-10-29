/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5file.h"
#include "gt_h5group.h"
#include "gt_h5dataset.h"
#include "gt_h5attribute.h"
#include "gt_h5reference.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Reference : public testing::Test
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
                                            GtH5Data<double>(), 0);
        ASSERT_TRUE(attribute.isValid());
    }

    GtH5File file;
    GtH5Group group;
    GtH5DataSet dataset;
    GtH5Attribute attribute;
};

TEST_F(TestH5Reference, referenceGroup)
{
    // reference
    GtH5Reference ref = group.toReference();
    EXPECT_TRUE(ref.isValid());
    EXPECT_EQ(ref.type(), group.type());

    // dereference
    GtH5Group grp = ref.toGroup(file);
    EXPECT_TRUE(grp.isValid());

    EXPECT_EQ(grp.path(), group.path());
    EXPECT_EQ(grp.name(), group.name());
    EXPECT_EQ(grp.file(), group.file());
}

TEST_F(TestH5Reference, referenceDataset)
{
    // reference
    GtH5Reference ref = dataset.toReference();
    EXPECT_TRUE(ref.isValid());
    EXPECT_EQ(ref.type(), dataset.type());

    // dereference
    GtH5DataSet dset = ref.toDataSet(file);
    EXPECT_TRUE(dset.isValid());

    EXPECT_EQ(dset.path(), dataset.path());
    EXPECT_EQ(dset.name(), dataset.name());
    EXPECT_EQ(dset.file(), dataset.file());
}

TEST_F(TestH5Reference, referenceAttribute)
{
    // datatset
//    GtH5Reference ref (dataset, attribute.name());
    GtH5Reference ref = attribute.toReference();
    EXPECT_TRUE(ref.isValid());
    EXPECT_EQ(ref.type(), attribute.type());

    // dereference
    GtH5Attribute attr = ref.toAttribute(file);
    EXPECT_TRUE(attr.isValid());

    EXPECT_EQ(attr.path(), attribute.path());
    EXPECT_EQ(attr.name(), attribute.name());
    EXPECT_EQ(attr.file(), attribute.file());
}



