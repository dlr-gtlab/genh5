/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_file.h"
#include "genh5_group.h"

#include "testhelper.h"

#include <QDir>
#include <QDebug>
#include <QProcess>

/// This is a test fixture that does a init for each test
class TestH5File : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        filePath = h5TestHelper->newFilePath();
        qfilePath = QString::fromStdString(filePath);
    }

    GenH5::String filePath;
    QString qfilePath;
};

TEST_F(TestH5File, isValid)
{
    GenH5::File file;
    // default file object should be invalid
    EXPECT_FALSE(file.isValid());
    // create a new file
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_TRUE(file.isValid());
}

TEST_F(TestH5File, root)
{
    GenH5::File file;
    // default file object should be invalid
    EXPECT_FALSE(file.isValid());
    // root group should also be invalid
    EXPECT_THROW(file.root(), GenH5::GroupException);

    // create a new file
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_TRUE(file.isValid());
    // root group should be valid
    GenH5::Group root = file.root();
    EXPECT_TRUE(root.isValid());

    // handles should be different
    EXPECT_NE(file.id(), root.id());
    EXPECT_EQ(file.id(), root.file().id());
}

TEST_F(TestH5File, filePath)
{
    GenH5::File file;
    // default file object should not have a filepath or filename associated
    EXPECT_EQ(file.fileName(), GenH5::String{});
    EXPECT_EQ(file.fileBaseName(), GenH5::String{});
    EXPECT_EQ(file.filePath(), GenH5::String{});

    // a created file should have the valid filename and filepath associated
    auto fileName = qfilePath.split('/').last();
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_EQ(file.filePath(), filePath);
    EXPECT_EQ(file.fileName(), fileName.toStdString());
    EXPECT_EQ(file.fileBaseName() + ".h5", fileName.toStdString());
}

TEST_F(TestH5File, creation)
{
    GenH5::File file;
    // invalid file may be closed without raising any exception
    EXPECT_NO_THROW(file.close());

    // file does not exist
    // Open -> fail
    EXPECT_THROW(GenH5::File(filePath, GenH5::Open),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(qfilePath));

    EXPECT_THROW(GenH5::File(filePath, { GenH5::Open | GenH5::ReadOnly }),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(qfilePath));

    qDebug() << "### EXPECTING ERROR: unable to open file";
    EXPECT_THROW(GenH5::File(filePath, { GenH5::Open | GenH5::ReadWrite }),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(qfilePath));
    qDebug() << "### END";

    // create new file
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(qfilePath));

    EXPECT_THROW(GenH5::File(filePath, GenH5::Create),
                 GenH5::FileException);

    EXPECT_TRUE(QFileInfo::exists(qfilePath));
    EXPECT_NO_THROW(file.close()); // clear file

    // file will be overwritten
    file = GenH5::File(filePath, GenH5::Overwrite);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(qfilePath));

    // file must be closed
    EXPECT_NO_THROW(file.close());
    EXPECT_FALSE(file.isValid());

    QDir dir{h5TestHelper->tempPath()};
    ASSERT_TRUE(dir.remove(qfilePath));

    // file does not exist -> fail
    EXPECT_THROW(GenH5::File(filePath, GenH5::Open),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(qfilePath));

    file = GenH5::File(filePath, { GenH5::Open | GenH5::Create });
    ASSERT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(qfilePath));
}
