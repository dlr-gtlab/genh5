/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
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
    }

    QByteArray filePath;
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
    EXPECT_EQ(file.fileName(), QString{});
    EXPECT_EQ(file.fileBaseName(), QString{});
    EXPECT_EQ(file.filePath(), QByteArray{});

    // a created file should have the valid filename and filepath associated
    auto fileName = filePath.split('/').last();
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_EQ(file.filePath(), filePath);
    EXPECT_EQ(file.fileName(), fileName);
    EXPECT_EQ(file.fileBaseName() + GenH5::File::dotFileSuffix(), fileName);
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
    ASSERT_FALSE(QFileInfo::exists(filePath));

    EXPECT_THROW(GenH5::File(filePath, { GenH5::Open | GenH5::ReadOnly }),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(filePath));

    qDebug() << "### EXPECTING ERROR: unable to open file";
    EXPECT_THROW(GenH5::File(filePath, { GenH5::Open | GenH5::ReadWrite }),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(filePath));
    qDebug() << "### END";

    // create new file
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(filePath));

    EXPECT_THROW(GenH5::File(filePath, GenH5::Create),
                 GenH5::FileException);

    EXPECT_TRUE(QFileInfo::exists(filePath));
    EXPECT_NO_THROW(file.close()); // clear file

    // file will be overwritten
    file = GenH5::File(filePath, GenH5::Overwrite);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(filePath));

    // file must be closed
    EXPECT_NO_THROW(file.close());
    EXPECT_FALSE(file.isValid());

    QDir dir{h5TestHelper->tempPath()};
    ASSERT_TRUE(dir.remove(filePath));

    // file does not exist -> fail
    EXPECT_THROW(GenH5::File(filePath, GenH5::Open),
                 GenH5::FileException);
    ASSERT_FALSE(QFileInfo::exists(filePath));

    file = GenH5::File(filePath, { GenH5::Open | GenH5::Create });
    ASSERT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(filePath));
}

TEST_F(TestH5File, fileSuffix)
{
    EXPECT_FALSE(GenH5::File::fileSuffix().isEmpty());
    EXPECT_FALSE(GenH5::File::dotFileSuffix().isEmpty());
}
