/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_FILE_H
#define GENH5_FILE_H

#include "genh5_object.h"
#include "genh5_group.h"

// forward decl
class QFile;

namespace GenH5
{

/**
 * @brief The AccessFlag enum
 */
enum FileAccessFlag
{
    Create = 1,
    Open = 2,
    Overwrite = 4,
    ReadOnly = 8,
    ReadWrite = 16
};
Q_DECLARE_FLAGS(FileAccessFlags, FileAccessFlag)

/**
 * @brief The File class
 */
class GENH5_EXPORT File : public Object
{
public:

    /**
     * @brief A default hdf5 file suffix not including dot
     * @return file suffix
     */
    static String fileSuffix() noexcept;
    /**
     * @brief same as fileSuffix including dot
     * @return file suffix
     */
    static String dotFileSuffix() noexcept;

    /**
     * @brief File
     */
    File();
    explicit File(hid_t id);
    explicit File(String path, FileAccessFlags flags = ReadWrite);

    /**
     * @brief id or handle of the hdf5 resource.
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief returns the root group of the file. The resulting object should
     * be valid as long as this object is valid.
     * @return root group of the file
     */
    Group& root() noexcept(false);
    Group const& root() const noexcept(false);

    /**
     * @brief fileName including file suffix.
     * @return filename
     */
    String fileName() const noexcept;

    /**
     * @brief file base name not including suffix.
     * @return filename
     */
    String fileBaseName() const noexcept;

    /**
     * @brief file path used to create this file.
     * @return filepath
     */
    String filePath() const noexcept;

    /**
     * @brief explicitly closes the resource handle.
     */
    void close();

    /// swaps all members
    void swap(File& other) noexcept;

private:

    /// file id
    IdComponent<H5I_FILE> m_id;
    /// caches the associated root group
    mutable Group m_root{};
};

GENH5_EXPORT GenH5::String getFileName(File const& file) noexcept;

} // namespace GenH5

inline void
swap(GenH5::File& a, GenH5::File& b) noexcept
{
    a.swap(b);
}

#endif // GENH5_FILE_H
