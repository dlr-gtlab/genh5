/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "testhelper.h"

#include "gt_h5file.h"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QUuid>

TestHelper*
TestHelper::instance()
{
    static TestHelper* instance = new TestHelper();
    return instance;
}

QString
TestHelper::newFilePath() const
{
    QDir dir(tempPath());

    if (!dir.mkpath(dir.absolutePath()))
    {
        qCritical() << "Error: could not create temp dir!";
        return QString();
    }

    return dir.absoluteFilePath(QUuid::createUuid().toString() +
                                GtH5File::fileSuffix());
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
