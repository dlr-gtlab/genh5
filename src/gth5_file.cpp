/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_file.h"
#include "gth5_group.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>

uint32_t
GtH5File::accessMode(AccessFlag mode)
{
    switch (mode)
    {
        case GtH5File::CreateReadWrite:
        case GtH5File::OpenReadWrite:
            return H5F_ACC_RDWR;
        case GtH5File::CreateOverwrite:
            return H5F_ACC_TRUNC;
        case GtH5File::CreateNotExisting:
            return H5F_ACC_EXCL;
        // default should be readonly to prevent data corruption
        case GtH5File::OpenReadOnly:
            break;
    }

    return H5F_ACC_RDONLY;
}

GtH5File::GtH5File() = default;

GtH5File::GtH5File(QFile const& file, AccessFlag flag) :
    GtH5File(QFileInfo(file).filePath().toUtf8(), flag)
{

}

GtH5File::GtH5File(QString const& path, AccessFlag flag) :
    GtH5File(path.toUtf8(), flag)
{

}

GtH5File::GtH5File(QByteArray const& path, AccessFlag flag) :
    m_filePath(path)
{
    QFileInfo fileInfo(path);
    QDir fileDir(fileInfo.path());

    if (!fileDir.exists())
    {
        qCritical() << "HDF5: Accessing file failed! (dir does not exist) -"
                    << path;
        return;
    }

    if (!fileInfo.exists())
    {
        if (flag == OpenReadOnly || flag == OpenReadWrite)
        {
            qCritical() << "HDF5: Opening file failed! (file does not exist) -"
                        << path;
            return;
        }

        if (flag == GtH5File::CreateOverwrite ||
            flag == GtH5File::CreateReadWrite)
        {
            flag =  GtH5File::CreateNotExisting;
        }
    }
    else if (flag == GtH5File::CreateNotExisting)
    {
        qCritical() << "HDF5: Creating file failed! (file already exists) -"
                    << path;
        return;
    }

    try
    {
        m_file = H5::H5File(path.constData(), accessMode(flag));
    }
    catch (H5::FileIException& /*e*/)
    {
        qCritical() << "HDF5: Accessing file failed!"
                    << path << flag;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5File:GtH5File failed!"
                    << path << flag;
    }
}

GtH5File::GtH5File(GtH5File const& other) :
    m_file{other.m_file},
    m_filePath{other.m_filePath},
    m_root{other.m_root}
{
//    qDebug() << "GtH5File::copy";
}

GtH5File::GtH5File(GtH5File&& other) noexcept :
    m_file{std::move(other.m_file)},
    m_filePath{std::move(other.m_filePath)},
    m_root{std::move(other.m_root)}
{
//    qDebug() << "GtH5File::move";
}

GtH5File&
GtH5File::operator=(GtH5File const& other)
{
//    qDebug() << "GtH5File::copy=";
    auto tmp{other};
    swap(tmp);
    return *this;
}

GtH5File&
GtH5File::operator=(GtH5File&& other) noexcept
{
//    qDebug() << "GtH5File::move=";
    swap(other);
    return *this;
}

bool
GtH5File::fileExists(const QString& path)
{
    return QFileInfo::exists(path);
}

bool
GtH5File::isValidH5File(const QString& filePath)
{
    return isValidH5File(filePath.toUtf8());
}

bool
GtH5File::isValidH5File(const QByteArray& filePath)
{
    return fileExists(filePath) &&
           H5::H5File::isAccessible(filePath.constData()) &&
           H5::H5File::isHdf5(filePath.constData());
}

int64_t
GtH5File::id() const
{
    return m_file.getId();
}

GtH5Group
GtH5File::root() const
{
    if (!m_root.isValid())
    {
        m_root = GtH5Group(*(const_cast<GtH5File*>(this)));
    }
    return m_root;
}

QByteArray
GtH5File::fileName() const
{
    return QFileInfo(m_filePath).fileName().toUtf8();
}

QByteArray
GtH5File::fileBaseName() const
{
    return QFileInfo(m_filePath).baseName().toUtf8();
}

QByteArray const&
GtH5File::filePath() const
{
    return m_filePath;
}

QByteArray
GtH5File::fileSuffix()
{
    return QByteArrayLiteral("h5");
}

QByteArray
GtH5File::dotFileSuffix()
{
    return QByteArrayLiteral(".h5");
}

void
GtH5File::close()
{
    m_file.close();
}

void
GtH5File::swap(GtH5File& other) noexcept
{
    using std::swap;
    swap (m_file, other.m_file);
    swap (m_filePath, other.m_filePath);
    swap (m_root, other.m_root);
}

H5::H5File
GtH5File::toH5() const
{
    return m_file;
}

void
swap(GtH5File& first, GtH5File& other) noexcept
{
    first.swap(other);
}
