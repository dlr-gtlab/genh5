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
    }

    QByteArray filePath;
};

TEST_F(TestH5File, isValid)
{
    GenH5::File file;
    // default file object should be invalid
    EXPECT_FALSE(file.isValid());
    // file should not exist and therefore not be valid
#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    EXPECT_FALSE(GenH5::File::isValidH5File(filePath));
#endif
    // create a new file
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_TRUE(file.isValid());
    // file path should exist and be valid
#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    EXPECT_TRUE(GenH5::File::isValidH5File(filePath));
#endif
}

TEST_F(TestH5File, root)
{
    GenH5::File file;
    // default file object should be invalid
    EXPECT_FALSE(file.isValid());
    // root group should also be invalid
//    GenH5::Group root = file.root();
//    EXPECT_FALSE(root.isValid());
    EXPECT_THROW(file.root(), GenH5::GroupException);

    // create a new file
    file = GenH5::File(filePath, GenH5::Create);
    EXPECT_TRUE(file.isValid());
    // root group should be valid
    GenH5::Group root = file.root();
    EXPECT_TRUE(root.isValid());

    // handles should be different
    EXPECT_NE(file.id(), root.id());
    EXPECT_EQ(file.id(), root.file()->id());
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
    // file does not exist
    // OpenReadOnly -> fail
//    file = GenH5::File(filePath, GenH5::OpenOnly);
//    EXPECT_FALSE(file.isValid());
//    ASSERT_FALSE(file.fileExists(filePath));
    try
    {
        file = GenH5::File(filePath, GenH5::Open);
        EXPECT_TRUE(false);
    }
    catch (GenH5::FileException const&) { EXPECT_TRUE(true); }
    EXPECT_FALSE(file.isValid());
    ASSERT_FALSE(QFileInfo::exists(filePath));

    // OpenReadWrite -> fail
//    file = GenH5::File(filePath, { GenH5::OpenOnly | GenH5::ReadOnly });
//    EXPECT_FALSE(file.isValid());
//    ASSERT_FALSE(file.fileExists(filePath));
    try
    {
        file = GenH5::File(filePath, { GenH5::Open | GenH5::ReadOnly });
        EXPECT_TRUE(false);
    }
    catch (GenH5::FileException const&) { EXPECT_TRUE(true); }
    EXPECT_FALSE(file.isValid());
    ASSERT_FALSE(QFileInfo::exists(filePath));

    // invalid file may be closed without raising any exception
    EXPECT_NO_THROW(file.close());

    // create new file
    file = GenH5::File(filePath, { GenH5::Create });
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(filePath));
    // file does already exist -> fail
//    file = GenH5::File(filePath, GenH5::CreateOnly);
    try
    {
        file = GenH5::File(filePath, GenH5::Create);
        EXPECT_TRUE(false);
    }
    catch (GenH5::FileException const&) { EXPECT_TRUE(true); }
    EXPECT_TRUE(file.isValid()); // old file handle is still valid
    EXPECT_TRUE(QFileInfo::exists(filePath));
    file.close(); // clear file

    // file will be overwritten
    file = GenH5::File(filePath, GenH5::Overwrite);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(filePath));

    // file must be closed
    EXPECT_NO_THROW(file.close());

    QDir dir{h5TestHelper->tempPath()};
    ASSERT_TRUE(dir.remove(filePath));

    ASSERT_FALSE(QFileInfo::exists(filePath));
    // file does not exist -> fail
//    file = GenH5::File(filePath, GenH5::OpenOnly);
    try
    {
        file = GenH5::File(filePath, GenH5::Open);
        EXPECT_TRUE(false);
    }
    catch (GenH5::FileException const&) { EXPECT_TRUE(true); }
    ASSERT_FALSE(file.isValid());
    ASSERT_FALSE(QFileInfo::exists(filePath));

    file = GenH5::File(filePath, { GenH5::Open | GenH5::Create });
    ASSERT_TRUE(file.isValid());
    EXPECT_TRUE(QFileInfo::exists(filePath));
}

TEST_F(TestH5File, fileLock)
{
    QByteArray fileName;
    QDir dir{filePath};

    // test scope
    {
        // cretae file instance
        GenH5::File file(filePath, GenH5::Create);
        ASSERT_TRUE(file.isValid());

#ifdef Q_OS_WIN
        // file cannot be deleted on windows if its still accessed
        EXPECT_TRUE(dir.exists(filePath));
        EXPECT_FALSE(dir.remove(filePath));
        EXPECT_TRUE(dir.exists(filePath));
#else
        fileName = file.fileName();
        // file is locked in linux
        QProcess process;
        process.start("lslocks", QStringList());
        ASSERT_TRUE(process.waitForFinished());
        EXPECT_TRUE(process.readAllStandardOutput().contains(fileName));
#endif
    }

    // file instance is destroyed and access released
#ifdef Q_OS_WIN
    // file can be deleted
    EXPECT_TRUE(dir.exists(filePath));
    EXPECT_TRUE(dir.remove(filePath));
    EXPECT_FALSE(dir.exists(filePath));
#else
    // file is no longer locked in linux
    QProcess process;
    process.start("lslocks", QStringList());
    ASSERT_TRUE (process.waitForFinished());
    EXPECT_FALSE(process.readAllStandardOutput().contains(fileName));
#endif
}
