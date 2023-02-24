/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_private.h"

#include <QFileInfo>
#include <QDir>

#include "H5Fpublic.h"
#include "H5Ppublic.h"

GenH5::String
GenH5::getFileName(File const& file) noexcept
{
    return GenH5::details::getName(file.id(),
                                   [](hid_t id, size_t len, char* data) {
         return H5Fget_name(id, data, len);
    });
}

GenH5::File::File() = default;

GenH5::File::File(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

GenH5::File::File(String path, FileAccessFlags flags)
{
    QFileInfo fileInfo{path};
    QDir fileDir{fileInfo.path()};

    if (!fileDir.exists())
    {
        throw FileException{
            GENH5_MAKE_EXECEPTION_STR()
            "Accessing file failed (directory does not exist: " +
            path.toStdString() + ')'
        };
    }

    uint flag = H5F_ACC_DEFAULT;
    bool exists = fileInfo.exists();
    bool create = false;

    if (flags & Overwrite)
    {
        create = true;
        flag = H5F_ACC_TRUNC;
    }
    else if (flags & ReadWrite)
    {
        flag = H5F_ACC_RDWR;
    }
    else if (!exists)
    {
        create = true;
        if (flags & Create)
        {
            flags &= ~Open;
            flag = H5F_ACC_EXCL;
        }
        if (flags & ReadOnly || flags & Open)
        {
            throw FileException{
                GENH5_MAKE_EXECEPTION_STR()
                "Opening file failed (file does not exist: " +
                path.toStdString() + ')'
            };
        }
    }
    else
    {
        if (flags & Open)
        {
            flag = H5F_ACC_RDWR;
        }
        else if (flags & Create)
        {
            throw FileException{
                GENH5_MAKE_EXECEPTION_STR()
                "Creating file failed (file already exists: " +
                path.toStdString() + ')'
            };
        }
        if (flags & ReadOnly)
        {
            flag = H5F_ACC_RDONLY;
        }
    }

    if (create)
    {
        m_id = H5Fcreate(path.constData(), flag, H5P_DEFAULT, H5P_DEFAULT);
        if (m_id < 0)
        {
            throw FileException{
                GENH5_MAKE_EXECEPTION_STR() "Failed to create file (path: " +
                path.toStdString() + ')'
            };
        }
    }
    else
    {
        m_id = H5Fopen(path.constData(), flag, H5P_DEFAULT);
        if (m_id < 0)
        {
            throw FileException{
                GENH5_MAKE_EXECEPTION_STR() "Failed to open file (path: " +
                path.toStdString() + ')'
            };
        }
    }
}

hid_t
GenH5::File::id() const noexcept
{
    return m_id;
}

GenH5::Group&
GenH5::File::root() noexcept(false)
{
    if (!isValid(m_root.id()))
    {
        m_root = Group{*this};
    }
    return m_root;
}

GenH5::Group const&
GenH5::File::root() const noexcept(false)
{
    return const_cast<File*>(this)->root();
}

GenH5::String
GenH5::File::fileName() const noexcept
{
    return QFileInfo{filePath()}.fileName().toUtf8();
}

GenH5::String
GenH5::File::fileBaseName() const noexcept
{
    return QFileInfo{filePath()}.baseName().toUtf8();
}

GenH5::String
GenH5::File::filePath() const noexcept
{
    return getFileName(*this);
}

GenH5::String
GenH5::File::fileSuffix() noexcept
{
    return QByteArrayLiteral("h5");
}

GenH5::String
GenH5::File::dotFileSuffix() noexcept
{
    return QByteArrayLiteral(".h5");
}

void
GenH5::File::close()
{
    if (m_root.isValid())
    {
        m_root.close();
    }
    m_id.dec();
}

void
GenH5::File::swap(File& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
    swap(m_root, other.m_root);
}
