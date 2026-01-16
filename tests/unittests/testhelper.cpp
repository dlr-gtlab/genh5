/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "testhelper.h"

#include "genh5_file.h"

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDir>
#include <QUuid>

TestHelper*
TestHelper::instance()
{
    static TestHelper instance{};
    return &instance;
}

QByteArray
TestHelper::newFilePath() const
{
    return newFilePath(QUuid::createUuid().toString() +
                       GenH5::File::dotFileSuffix());
}

QByteArray
TestHelper::newFilePath(QString fileName) const
{
    QDir dir(tempPath());

    if (!dir.mkpath(dir.absolutePath()))
    {
        qCritical() << "Error: could not create temp dir!";
        return {};
    }

    return dir.absoluteFilePath(fileName).toUtf8();
}

QByteArray
TestHelper::newFilePath(testing::TestInfo* testInfo) const
{
    assert(testInfo);
    return newFilePath(QString{testInfo->test_case_name()} + "_" +
                       QString{testInfo->name()} +
                       GenH5::File::dotFileSuffix());
}

QString
TestHelper::tempPath() const
{
    return QCoreApplication::applicationDirPath() + QDir::separator() +
            QStringLiteral("unittest-temp");
}

void
TestHelper::reset()
{
    QDir tempDir(tempPath());

    if (tempDir.exists())
    {
        tempDir.removeRecursively();
    }
}

QStringList
TestHelper::randomStringList(int length) const
{
    QStringList retVal;
    retVal.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        retVal.append(QUuid::createUuid().toString());
    }

    return retVal;
}

QVector<QByteArray>
TestHelper::randomByteArrays(int length) const
{
    QVector<QByteArray> retVal;
    retVal.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        retVal.append(QUuid::createUuid().toString().toUtf8());
    }

    return retVal;
}
