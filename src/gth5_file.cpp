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

GtH5::String
GtH5::getFileName(File const& file)
{
    if (!file.isValid())
    {
        return {};
    }

    String buffer{32, ' '};
    size_t bufferLen = static_cast<size_t>(buffer.size());

    auto acutalLen = static_cast<size_t>(H5Fget_name(file.id(),
                                                     buffer.data(),
                                                     bufferLen));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(static_cast<int>(bufferLen));
        H5Fget_name(file.id(), buffer.data(), bufferLen);
    }

    // remove of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
uint32_t
GtH5::File::accessMode(AccessFlag mode)
{
    switch (mode)
    {
        case GtH5::File::CreateReadWrite:
        case GtH5::File::OpenReadWrite:
            return H5F_ACC_RDWR;
        case GtH5::File::CreateOverwrite:
            return H5F_ACC_TRUNC;
        case GtH5::File::CreateNotExisting:
            return H5F_ACC_EXCL;
        // default should be readonly to prevent data corruption
        case GtH5::File::OpenReadOnly:
            break;
    }

    return H5F_ACC_RDONLY;
}
#endif

uint32_t
GtH5::File::accessMode(AccessFlags flags)
{
    switch (flags)
    {
    case GtH5::Overwrite:
        return H5F_ACC_TRUNC;
    case GtH5::CreateOnly:
        return H5F_ACC_EXCL; // fail if file exist
    case GtH5::OpenOnly:
    case GtH5::ReadWrite:
        return H5F_ACC_RDWR;
    case GtH5::ReadOnly:
        return H5F_ACC_RDONLY;
    }

    return H5F_ACC_DEFAULT;
}

GtH5::File::File() = default;

GtH5::File::File(QFile const& file, AccessFlags flags) :
    File{QFileInfo(file).filePath().toUtf8(), flags}
{

}

GtH5::File::File(String path, AccessFlags flags)
{
    QFileInfo fileInfo{path};
    QDir fileDir{fileInfo.path()};

    if (!fileDir.exists())
    {
        qCritical() << "HDF5: Accessing file failed! (dir does not exist) -"
                    << path;
        return;
    }

    if (!fileInfo.exists())
    {
        if ((flags & OpenOnly))
        {
            qCritical() << "HDF5: Opening file failed! (file does not exist) -"
                        << path;
            return;
        }
    }
    else
    {
        if ((flags & CreateOnly))
        {
            qCritical() << "HDF5: Creating file failed! (file already exist) -"
                        << path;
            return;
        }
    }

    try
    {
        m_file = H5::H5File{path.constData(), accessMode(flags)};
    }
    catch (H5::FileIException& /*e*/)
    {
        qCritical() << "HDF5: Accessing file failed!"
                    << path << flags;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::File:GtH5File failed!"
                    << path << flags;
    }
}

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
GtH5::File::File(QFile const& file, AccessFlag flag) :
    File{QFileInfo(file).filePath().toUtf8(), flag}
{

}

GtH5::File::File(QString const& path, AccessFlag flag) :
    File{path.toUtf8(), flag}
{

}

GtH5::File::File(String path, AccessFlag flag)
{
    QFileInfo fileInfo{path};
    QDir fileDir{fileInfo.path()};

    if (!fileDir.exists())
    {
        qCritical() << "HDF5: Accessing file failed! (dir does not exist) -"
                    << path;
        return;
    }

//    H5F_ACC_EXCL

    if (!fileInfo.exists())
    {
        if (flag == OpenReadOnly || flag == OpenReadWrite)
        {
            qCritical() << "HDF5: Opening file failed! (file does not exist) -"
                        << path;
            return;
        }

        if (flag == CreateOverwrite ||
            flag == CreateReadWrite)
        {
            flag =  CreateNotExisting;
        }
    }
    else if (flag == CreateNotExisting)
    {
        qCritical() << "HDF5: Creating file failed! (file already exists) -"
                    << path;
        return;
    }

    try
    {
        m_file = H5::H5File{path.constData(), accessMode(flag)};
    }
    catch (H5::FileIException& /*e*/)
    {
        qCritical() << "HDF5: Accessing file failed!"
                    << path << flag;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::File:GtH5File failed!"
                    << path << flag;
    }
}
#endif

bool
GtH5::File::fileExists(QString const& path)
{
    return QFileInfo::exists(path);
}

bool
GtH5::File::isValidH5File(QString const& filePath)
{
    return isValidH5File(filePath.toUtf8());
}

bool
GtH5::File::isValidH5File(String const& filePath)
{
    return fileExists(filePath) &&
           H5::H5File::isAccessible(filePath.constData()) &&
           H5::H5File::isHdf5(filePath.constData());
}

hid_t
GtH5::File::id() const
{
    return m_file.getId();
}

GtH5::Group
GtH5::File::root() const
{
    if (!isValid(m_root.id()))
    {
        m_root = Group{*this};
    }
    return m_root;
}

GtH5::String
GtH5::File::fileName() const
{
    return QFileInfo{getFileName(*this)}.fileName().toUtf8();
}

GtH5::String
GtH5::File::fileBaseName() const
{
    return QFileInfo{getFileName(*this)}.baseName().toUtf8();
}

GtH5::String
GtH5::File::filePath() const
{
    return getFileName(*this);
}

GtH5::String
GtH5::File::fileSuffix()
{
    return QByteArrayLiteral("h5");
}

GtH5::String
GtH5::File::dotFileSuffix()
{
    return QByteArrayLiteral(".h5");
}

void
GtH5::File::close()
{
    m_file.close();
    if (m_root.file())
    {
        m_root.file()->m_file.close();
    }
}

H5::H5File const&
GtH5::File::toH5() const
{
    return m_file;
}
