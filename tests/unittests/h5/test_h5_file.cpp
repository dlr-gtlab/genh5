/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_file.h"
#include "gth5_group.h"

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

    QString filePath;
};

TEST_F(TestH5File, isValid)
{
    GtH5::File file;
    // default file object should be invalid
    EXPECT_FALSE(file.isValid());
    // file should not exist and therefore not be valid
    EXPECT_FALSE(GtH5::File::isValidH5File(filePath));

    // create a new file
    file = GtH5::File(filePath, GtH5::Create);
    EXPECT_TRUE(file.isValid());
    // file path should exist and be valid
    EXPECT_TRUE(GtH5::File::isValidH5File(filePath));
}

TEST_F(TestH5File, root)
{
    GtH5::File file;
    // default file object should be invalid
    EXPECT_FALSE(file.isValid());
    // root group should also be invalid
//    GtH5::Group root = file.root();
//    EXPECT_FALSE(root.isValid());
    EXPECT_THROW(file.root(), GtH5::GroupException);

    // create a new file
    file = GtH5::File(filePath, GtH5::Create);
    EXPECT_TRUE(file.isValid());
    // root group should be valid
    GtH5::Group root = file.root();
    EXPECT_TRUE(root.isValid());

    // handles should be different
    EXPECT_NE(file.id(), root.id());
    EXPECT_EQ(file.id(), root.file()->id());
}

TEST_F(TestH5File, filePath)
{
    GtH5::File file;
    // default file object should not have a filepath or filename associated
    EXPECT_EQ(file.fileName(), QString{});
    EXPECT_EQ(file.fileBaseName(), QString{});
    EXPECT_EQ(file.filePath(), QByteArray{});

    // a created file should have the valid filename and filepath associated
    QString fileName = filePath.split(QStringLiteral("/")).last();
    file = GtH5::File(filePath, GtH5::Create);
    EXPECT_EQ(file.filePath(), filePath);
    EXPECT_EQ(file.fileName(), fileName);
    EXPECT_EQ(file.fileBaseName() + GtH5::File::dotFileSuffix(), fileName);
}

TEST_F(TestH5File, creation)
{
    GtH5::File file;
    // file does not exist
    // OpenReadOnly -> fail
//    file = GtH5::File(filePath, GtH5::OpenOnly);
//    EXPECT_FALSE(file.isValid());
//    ASSERT_FALSE(file.fileExists(filePath));
    try
    {
        file = GtH5::File(filePath, GtH5::Open);
        EXPECT_TRUE(false);
    }
    catch (GtH5::FileException const&) { EXPECT_TRUE(true); }
    EXPECT_FALSE(file.isValid());
    ASSERT_FALSE(file.fileExists(filePath));

    // OpenReadWrite -> fail
//    file = GtH5::File(filePath, { GtH5::OpenOnly | GtH5::ReadOnly });
//    EXPECT_FALSE(file.isValid());
//    ASSERT_FALSE(file.fileExists(filePath));
    try
    {
        file = GtH5::File(filePath, { GtH5::Open | GtH5::ReadOnly });
        EXPECT_TRUE(false);
    }
    catch (GtH5::FileException const&) { EXPECT_TRUE(true); }
    EXPECT_FALSE(file.isValid());
    ASSERT_FALSE(file.fileExists(filePath));

    // invalid file may be closed without raising any exception
    EXPECT_NO_THROW(file.close());

    // create new file
    file = GtH5::File(filePath, { GtH5::Create });
    EXPECT_TRUE(file.isValid());
    ASSERT_TRUE(file.fileExists(filePath));
    // file does already exist -> fail
//    file = GtH5::File(filePath, GtH5::CreateOnly);
    try
    {
        file = GtH5::File(filePath, GtH5::Create);
        EXPECT_TRUE(false);
    }
    catch (GtH5::FileException const&) { EXPECT_TRUE(true); }
    EXPECT_TRUE(file.isValid()); // old file handle is still valid
    ASSERT_TRUE(file.fileExists(filePath));
    file.close(); // clear file

    // file will be overwritten
    file = GtH5::File(filePath, GtH5::Overwrite);
    EXPECT_TRUE(file.isValid());
    ASSERT_TRUE(file.fileExists(filePath));

    // file must be closed
    EXPECT_NO_THROW(file.close());

    QDir dir{h5TestHelper->tempPath()};
    ASSERT_TRUE(dir.remove(filePath));

    ASSERT_FALSE(file.fileExists(filePath));
    // file does not exist -> fail
//    file = GtH5::File(filePath, GtH5::OpenOnly);
    try
    {
        file = GtH5::File(filePath, GtH5::Open);
        EXPECT_TRUE(false);
    }
    catch (GtH5::FileException const&) { EXPECT_TRUE(true); }
    ASSERT_FALSE(file.isValid());
    ASSERT_FALSE(file.fileExists(filePath));
}

TEST_F(TestH5File, fileLock)
{
    QByteArray fileName;
    QDir dir{filePath};

    // test scope
    {
        // cretae file instance
        GtH5::File file(filePath, GtH5::Create);
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
