/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_file.h"
#include "gth5_group.h"
#include "gth5_dataset.h"
#include "gth5_attribute.h"
#include "gth5_reference.h"
#include "gth5_data.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Reference : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GtH5::File(h5TestHelper->newFilePath(),
                          GtH5::Create);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      GtH5::dataType<int>(),
                                      GtH5::DataSpace::Scalar);
        ASSERT_TRUE(dataset.isValid());

        attribute = dataset.createAttribute(QByteArrayLiteral("attribute"),
                                            GtH5::dataType<int>(),
                                            GtH5::DataSpace::Scalar);
        ASSERT_TRUE(attribute.isValid());
    }

    GtH5::File file;
    GtH5::Group group;
    GtH5::DataSet dataset;
    GtH5::Attribute attribute;
};

TEST_F(TestH5Reference, isValid)
{
    // reference
    GtH5::Reference ref;
    EXPECT_FALSE(ref.isValid());

    GtH5::Reference refA = group.toReference();
    EXPECT_TRUE(refA.isValid());

    GtH5::Reference refB = dataset.toReference();
    EXPECT_TRUE(refB.isValid());

    GtH5::Reference refC = attribute.toReference();
    EXPECT_TRUE(refC.isValid());

    GtH5::DataSet dset;
    GtH5::Reference refD = dset.toReference();
    EXPECT_FALSE(refD.isValid());
}

TEST_F(TestH5Reference, invalid)
{
    GtH5::Reference ref;

    EXPECT_THROW(ref.toGroup(file), GtH5::ReferenceException);
    EXPECT_THROW(ref.toDataSet(file), GtH5::ReferenceException);
    EXPECT_THROW(ref.toAttribute(file), GtH5::ReferenceException);
}

TEST_F(TestH5Reference, referenceGroup)
{
    // reference
    GtH5::Reference ref = group.toReference();

    // dereference
    GtH5::Group grp = ref.toGroup(file);
    EXPECT_TRUE(grp.isValid());

    EXPECT_EQ(grp.path(), group.path());
    EXPECT_EQ(grp.name(), group.name());
    EXPECT_EQ(grp.file(), group.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceDataset)
{
    // reference
    GtH5::Reference ref = dataset.toReference();

    // dereference
    GtH5::DataSet dset = ref.toDataSet(file);
    EXPECT_TRUE(dset.isValid());

    EXPECT_EQ(dset.path(), dataset.path());
    EXPECT_EQ(dset.name(), dataset.name());
    EXPECT_EQ(dset.file(), dataset.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceAttribute)
{
    // reference
    GtH5::Reference ref = attribute.toReference();

    // dereference
    GtH5::Attribute attr = ref.toAttribute(file);
    EXPECT_TRUE(attr.isValid());

    EXPECT_EQ(attr.path(), attribute.path());
    EXPECT_EQ(attr.name(), attribute.name());
    EXPECT_EQ(attr.file(), attribute.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceGroupAlign)
{
    // reference
    auto align =  group.toReference().alignment();

    // dereference
    GtH5::Group grp = GtH5::Reference(align).toGroup(file);
    EXPECT_TRUE(grp.isValid());

    EXPECT_EQ(grp.path(), group.path());
    EXPECT_EQ(grp.name(), group.name());
    EXPECT_EQ(grp.file(), group.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceDatasetAlign)
{
    // reference
    auto align =  dataset.toReference().alignment();

    // dereference
    GtH5::DataSet dset = GtH5::Reference(align).toDataSet(file);
    EXPECT_TRUE(dset.isValid());

    EXPECT_EQ(dset.path(), dataset.path());
    EXPECT_EQ(dset.name(), dataset.name());
    EXPECT_EQ(dset.file(), dataset.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceAttributeAlign)
{
    // reference
    auto align =  attribute.toReference().alignment();

    // dereference does not work for attributes when using alignment data
    EXPECT_THROW(GtH5::Reference(align).toAttribute(file),
                 GtH5::ReferenceException);

    //    qDebug() << "# Expect Error: invalid reference type";
//    GtH5::Attribute attr = GtH5::Reference(align).toAttribute(file);
//    EXPECT_FALSE(attr.isValid());

//    EXPECT_EQ(attr.path(), QByteArray{});
//    EXPECT_EQ(attr.name(), QByteArray{});
//    EXPECT_FALSE(attr.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceGroupBuffer)
{
    // reference
    auto buffer =  group.toReference().buffer();

    // dereference
    GtH5::Group grp = GtH5::Reference(buffer).toGroup(file);
    EXPECT_TRUE(grp.isValid());

    EXPECT_EQ(grp.path(), group.path());
    EXPECT_EQ(grp.name(), group.name());
    EXPECT_EQ(grp.file(), group.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceDatasetBuffer)
{
    // reference
    auto buffer =  dataset.toReference().buffer();

    // dereference
    GtH5::DataSet dset = GtH5::Reference(buffer).toDataSet(file);
    EXPECT_TRUE(dset.isValid());

    EXPECT_EQ(dset.path(), dataset.path());
    EXPECT_EQ(dset.name(), dataset.name());
    EXPECT_EQ(dset.file(), dataset.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceAttributeBuffer)
{
    // reference
    auto buffer =  attribute.toReference().buffer();

    // dereference
    GtH5::Attribute attr = GtH5::Reference(buffer).toAttribute(file);
    EXPECT_TRUE(attr.isValid());

    EXPECT_EQ(attr.path(), attribute.path());
    EXPECT_EQ(attr.name(), attribute.name());
    EXPECT_EQ(attr.file(), attribute.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}
