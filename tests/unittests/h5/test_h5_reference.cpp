/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_dataset.h"
#include "genh5_attribute.h"
#include "genh5_reference.h"
#include "genh5_data.h"

#include "testhelper.h"

#include <QDebug>


/// This is a test fixture that does a init for each test
class TestH5Reference : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GenH5::File(h5TestHelper->newFilePath(),
                          GenH5::Create);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      GenH5::dataType<int>(),
                                      GenH5::DataSpace::Scalar);
        ASSERT_TRUE(dataset.isValid());

        attribute = dataset.createAttribute(QByteArrayLiteral("attribute"),
                                            GenH5::dataType<int>(),
                                            GenH5::DataSpace::Scalar);
        ASSERT_TRUE(attribute.isValid());
    }

    GenH5::File file;
    GenH5::Group group;
    GenH5::DataSet dataset;
    GenH5::Attribute attribute;
};

TEST_F(TestH5Reference, isValid)
{
    // reference
    GenH5::Reference ref;
    EXPECT_FALSE(ref.isValid());

    GenH5::Reference refA = group.toReference();
    EXPECT_TRUE(refA.isValid());

    GenH5::Reference refB = dataset.toReference();
    EXPECT_TRUE(refB.isValid());

    GenH5::Reference refC = attribute.toReference();
    EXPECT_TRUE(refC.isValid());

    GenH5::DataSet dset;
    EXPECT_THROW(dset.toReference(), GenH5::ReferenceException);
}

TEST_F(TestH5Reference, invalid)
{
    GenH5::Reference ref;

    EXPECT_THROW(ref.toGroup(file), GenH5::ReferenceException);
    EXPECT_THROW(ref.toDataSet(file), GenH5::ReferenceException);
    EXPECT_THROW(ref.toAttribute(file), GenH5::ReferenceException);
}

TEST_F(TestH5Reference, referenceGroup)
{
    // reference
    GenH5::Reference ref = group.toReference();

    // dereference
    GenH5::Group grp = ref.toGroup(file);
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
    GenH5::Reference ref = dataset.toReference();

    // dereference
    GenH5::DataSet dset = ref.toDataSet(file);
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
    GenH5::Reference ref = attribute.toReference();

    // dereference
    GenH5::Attribute attr = ref.toAttribute(file);
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
    GenH5::Group grp = GenH5::Reference(align).toGroup(file);
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
    GenH5::DataSet dset = GenH5::Reference(align).toDataSet(file);
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
    qDebug() << "### EXPECTEING ERROR: Bad value";
    EXPECT_THROW(GenH5::Reference(align).toAttribute(file),
                 GenH5::ReferenceException);
    qDebug() << "### END";

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}

TEST_F(TestH5Reference, referenceGroupBuffer)
{
    // reference
    auto buffer =  group.toReference().buffer();

    // dereference
    GenH5::Group grp = GenH5::Reference(buffer).toGroup(file);
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
    GenH5::DataSet dset = GenH5::Reference(buffer).toDataSet(file);
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
    GenH5::Attribute attr = GenH5::Reference(buffer).toAttribute(file);
    EXPECT_TRUE(attr.isValid());

    EXPECT_EQ(attr.path(), attribute.path());
    EXPECT_EQ(attr.name(), attribute.name());
    EXPECT_EQ(attr.file(), attribute.file());

    // local file and internal shared file ptr have access
    EXPECT_EQ(H5Iget_ref(file.id()), 2);
}
