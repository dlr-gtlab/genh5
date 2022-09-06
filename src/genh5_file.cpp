/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_file.h"
#include "genh5_group.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>

GenH5::String
GenH5::getFileName(File const& file) noexcept
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

GenH5::File::File() = default;

GenH5::File::File(H5::H5File file) :
    m_file{std::move(file)}
{ }

GenH5::File::File(String path, FileAccessFlags flags)
{
    QFileInfo fileInfo{path};
    QDir fileDir{fileInfo.path()};

    if (!fileDir.exists())
    {
        throw FileException{"Accessing file failed! "
                            "(Directory does not exist)"};
    }

    uint flag = H5F_ACC_DEFAULT;
    bool exists = fileInfo.exists();

    if (flags & Overwrite)
    {
        flag = H5F_ACC_TRUNC;
    }
    else if (flags & ReadWrite)
    {
        flag = H5F_ACC_RDWR;
    }
    else if (!exists)
    {
        if (flags & Create)
        {
            flags &= ~Open;
            flag = H5F_ACC_EXCL;
        }
        if (flags & ReadOnly || flags & Open)
        {
            throw FileException{"Opening file failed! "
                                "(File does not exist)"};
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
            throw FileException{"Creating file failed! "
                                "(File already exist)"};
        }
        if (flags & ReadOnly)
        {
            flag = H5F_ACC_RDONLY;
        }
    }

    try
    {
        m_file = H5::H5File{path.constData(), flag};
    }
    catch (H5::FileIException const& e)
    {
        throw FileException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] File::File";
        throw FileException{e.getCDetailMsg()};
    }
}

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
bool
GenH5::File::isValidH5File(String const& filePath)
{
    try
    {
        return QFileInfo::exists(filePath) &&
               H5::H5File::isAccessible(filePath.constData()) &&
               H5::H5File::isHdf5(filePath.constData());
    }
    catch (H5::Exception const& /*e*/)
    {
        return false;
    }
}
#endif

hid_t
GenH5::File::id() const noexcept
{
    return m_file.getId();
}

GenH5::Group
GenH5::File::root() const noexcept(false)
{
    if (!isValid(m_root.id()))
    {
        m_root = Group{*this};
    }
    return m_root;
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
    m_file.close();
    if (m_root.file())
    {
        m_root.file()->m_file.close();
    }
}

H5::H5File const&
GenH5::File::toH5() const noexcept
{
    return m_file;
}
