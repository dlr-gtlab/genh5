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
#include "gt_h5data.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Location : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GtH5File(TestHelper::instance()->newFilePath(),
                        GtH5File::CreateOverwrite);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());

        dataset = group.createDataset(QByteArrayLiteral("dataset"),
                                      GtH5Data<int>().dataType(),
                                      GtH5DataSpace{0});
        ASSERT_TRUE(dataset.isValid());

        attribute = dataset.createAttribute(QByteArrayLiteral("attribute"),
                                            GtH5Data<int>().dataType(),
                                            GtH5DataSpace{0});
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
    EXPECT_EQ(GtH5File().root().name(), QByteArrayLiteral("/"));
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
    EXPECT_EQ(GtH5File().root().file().get(), nullptr);
    EXPECT_EQ(GtH5Group().file().get(), nullptr);
    EXPECT_EQ(GtH5DataSet().file().get(), nullptr);
    EXPECT_EQ(GtH5Attribute().file().get(), nullptr);

    // root group creates a file on the heap
    void* filePtr = file.root().file().get();
    // so these pointer should not match
    EXPECT_NE(&file, filePtr);

    // however alls other objects should point to the same object on heap
    EXPECT_EQ(group.file().get(), filePtr);
    EXPECT_EQ(dataset.file().get(), filePtr);
    EXPECT_EQ(attribute.file().get(), filePtr);
}

TEST_F(TestH5Location, fileRefCount)
{
    int64_t id = 0;

    {
        GtH5Group _root;

        // file should be null
        ASSERT_EQ(_root.file().get(), nullptr);

        {
            auto _file = GtH5File(TestHelper::instance()->newFilePath(),
                                 GtH5File::CreateOverwrite);

            id = _file.id();

            // root object was not instantiated yet
            // -> only local file has access
            EXPECT_EQ(H5Iget_ref(id), 1);

            // this creates the root object
            // -> shared file is created which has also access now
            _root = _file.root();

            EXPECT_EQ(H5Iget_ref(id), 2);

            // file ids should be equal
            EXPECT_EQ(_root.file()->id(), _file.id());


            // local file will be deleted -> ref count will be decremented
        }

        // file should no longer be null
        ASSERT_NE(_root.file().get(), nullptr);
        // only root group has access
        EXPECT_EQ(H5Iget_ref(id), 1);

        // group _root will be deleted here -> ref count will be decremented
    }

    qDebug() << "# Expect Error: id not found";
    // file is no longer accessed
    // id cant be found -> returns -1
    EXPECT_EQ(H5Iget_ref(id), -1);
}

TEST_F(TestH5Location, exists)
{
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group/")));
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("group/dataset")));
    // leading slash should not matter
    EXPECT_TRUE(file.root().exists(QByteArrayLiteral("/group/dataset")));

    // this should not exist
    EXPECT_FALSE(file.root().exists(QByteArrayLiteral("dataset")));
    // however this should
    EXPECT_TRUE(group.exists(QByteArrayLiteral("dataset")));
}
