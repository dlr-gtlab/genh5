/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 05.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_dataset.h"
#include "genh5_attribute.h"

#include "testhelper.h"

#include <QDebug>


/// This is a test fixture that does a init for each test
class TestH5Location : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GenH5::File{h5TestHelper->newFilePath(),
                          GenH5::Create};
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataSet(QByteArrayLiteral("dataset"),
                                      GenH5::dataType<int>(),
                                      GenH5::DataSpace::Scalar());
        ASSERT_TRUE(dataset.isValid());

        attribute = dataset.createAttribute(QByteArrayLiteral("attribute"),
                                            GenH5::dataType<int>(),
                                            GenH5::DataSpace::Scalar());
        ASSERT_TRUE(attribute.isValid());
    }

    GenH5::File file;
    GenH5::Group group;
    GenH5::DataSet dataset;
    GenH5::Attribute attribute;
};

TEST_F(TestH5Location, linkName)
{
    EXPECT_THROW(GenH5::File().root().name(), GenH5::GroupException);

    EXPECT_EQ(GenH5::Group().name(), QByteArray{});
    EXPECT_EQ(GenH5::DataSet().name(), QByteArray{});
    EXPECT_EQ(GenH5::Attribute().name(), QByteArray{});

    EXPECT_EQ(file.root().name(), QByteArrayLiteral("/"));
    EXPECT_EQ(group.name(), QByteArrayLiteral("group"));
    EXPECT_EQ(dataset.name(), QByteArrayLiteral("dataset"));
    EXPECT_EQ(attribute.name(), QByteArrayLiteral("attribute"));
}

TEST_F(TestH5Location, linkPath)
{
    EXPECT_THROW(GenH5::File().root().path(), GenH5::GroupException);

    EXPECT_EQ(GenH5::Group().path(), QByteArray{});
    EXPECT_EQ(GenH5::DataSet().path(), QByteArray{});
    EXPECT_EQ(GenH5::Attribute().path(), QByteArray{});

    EXPECT_EQ(file.root().path(), QByteArrayLiteral("/"));
    EXPECT_EQ(group.path(), QByteArrayLiteral("/group"));
    EXPECT_EQ(dataset.path(), QByteArrayLiteral("/group/dataset"));
    EXPECT_EQ(attribute.path(), dataset.path());
}

TEST_F(TestH5Location, fileRefCount)
{
    hid_t id = 0;

    {
        GenH5::Group _root;

        {
            auto _file = GenH5::File(h5TestHelper->newFilePath(),
                                    GenH5::Create);

            id = _file.id();

            // root object was not instantiated yet
            // -> only local file has access
            EXPECT_EQ(GenH5::refCount(id), 1);

            // this creates the root object
            _root = _file.root();
            auto file2 = _root.file();

            EXPECT_EQ(GenH5::refCount(id), 2);

            // file ids should be equal
            EXPECT_EQ(_root.file().id(), _file.id());

            // local file will be deleted -> ref count will be decremented
        }

        qDebug() << "### EXPECTING ERROR: Id not found";
        // file is not open
        EXPECT_EQ(GenH5::refCount(id), -1);
        qDebug() << "### END";

        // group _root will be deleted here -> ref count will be decremented
    }

    qDebug() << "### EXPECTING ERROR: Id not found";
    // file is no longer accessed
    // id cant be found -> returns -1
    EXPECT_EQ(GenH5::refCount(id), -1);
    qDebug() << "### END";
}

TEST_F(TestH5Location, exists)
{
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group")));
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group/")));
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("/group/")));

    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group/dataset")));
    // leading slash should not matter
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("/group/dataset/")));

    // this should not exist
    EXPECT_FALSE(file.root().exists(QByteArrayLiteral("dataset")));
    // invalid path name
    EXPECT_FALSE(group.exists(QByteArrayLiteral("/dataset/")));
    // however this should exist
    EXPECT_TRUE(group.exists(QByteArrayLiteral("dataset/")));
}

TEST_F(TestH5Location, issue_137_group_name_exceeds_preallocated_buffer)
{
    // if location name does not
    auto file = GenH5::File("truncation.h5", GenH5::Overwrite);

    // all together 32 byte including 2 "/"
    auto group = file.root().createGroup("Root5678901234567890123456");
    auto testgroup = group.createGroup("test");

    EXPECT_EQ(QByteArrayLiteral("test"), testgroup.name());
    EXPECT_EQ(QByteArrayLiteral("/Root5678901234567890123456/test"), testgroup.path());
}
