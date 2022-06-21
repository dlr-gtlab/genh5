/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_FILE_H
#define GTH5_FILE_H

#include "gth5_object.h"
#include "gth5_group.h"

// forward decl
class QFile;

namespace GtH5
{

/**
 * @brief The AccessFlag enum
 */
enum AccessFlag
{
    CreateOnly = 1,
    OpenOnly = 2,
    Overwrite = 4,
    ReadOnly = 8,
    ReadWrite = 16
};
Q_DECLARE_FLAGS(AccessFlags, AccessFlag)


/**
 * @brief The File class
 */
class GTH5_EXPORT File : public Object
{
public:

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
    enum AccessFlag
    {
        OpenReadOnly = 1,
        OpenReadWrite = 2,
        CreateReadWrite = 4,
        CreateNotExisting = 8,
        CreateOverwrite = 16
    };

    static uint32_t accessMode(AccessFlag mode);
#endif
    /**
     * @brief translates the access flags to hdf5 compatible access flags
     * (eg. H5F_ACC_TRUNC).
     * @param flags access mode
     * @return hdf5 access mode
     */
    static uint32_t accessMode(AccessFlags flags);

    /**
     * @brief returns whether the file exists
     * @param path file path
     * @return whether the file exists
     */
    static bool fileExists(QString const& path);

    /**
     * @brief returns whether the file is a valid hdf5 file. Does not check if
     * file is corrupted
     * @param filePath file path
     * @return whether the file is a valid hdf5 file
     */
    static bool isValidH5File(QString const& filePath);
    static bool isValidH5File(String const& filePath);

    /**
     * @brief hdf5 file suffix not including dot
     * @return file suffix
     */
    static String fileSuffix();
    /**
     * @brief hdf5 file suffix including dot
     * @return file suffix
     */
    static String dotFileSuffix();

    /**
     * @brief File
     */
    File();
#ifndef GTH5_NO_DEPRECATED_SYMBOLS
    File(QFile const& h5File, AccessFlag flag);
    File(QString const& path, AccessFlag flag);
    File(String path, AccessFlag flag);
#endif
    explicit File(QFile const& h5File, AccessFlags flags = ReadWrite);
    explicit File(String path, AccessFlags flags = ReadWrite);

    /**
     * @brief allows access of the base hdf5 object.
     * @return base hdf5 object
     */
    H5::H5File const& toH5() const;

    /**
     * @brief id or handle of the hdf5 resource.
     * @return id
     */
    hid_t id() const override;

    /**
     * @brief returns the root group of the file. The resulting object should
     * be valid as long as this object is valid.
     * @return root group of the file
     */
    Group root() const;

    /**
     * @brief fileName including file suffix.
     * @return filename
     */
    String fileName() const;

    /**
     * @brief file base name not including suffix.
     * @return filename
     */
    String fileBaseName() const;

    /**
     * @brief file path used to create this file.
     * @return filepath
     */
    String const& filePath() const;

    /**
     * @brief explicitly closes the resource handle.
     */
    void close();

private:

    /// hdf5 base instance
    H5::H5File m_file{};
    /// stores the file path used to create this object
    String m_filePath{};
    /// cashes the associated root group
    mutable Group m_root{};
};

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5File = GtH5::File;
#endif

#endif // GTH5_FILE_H
