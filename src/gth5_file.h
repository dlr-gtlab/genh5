/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5FILE_H
#define GTH5FILE_H

#include "gth5_object.h"
#include "gth5_group.h"

#include <QByteArray>

class QFile;
/**
 * @brief The GtH5File class
 */
class GTH5_EXPORT GtH5File : public GtH5Object
{
public:

    /**
     * @brief The AccessFlag enum
     */
    enum AccessFlag
    {
        OpenReadOnly = 1,
        OpenReadWrite = 2,
        CreateReadWrite = 4,
        CreateNotExisting = 8,
        CreateOverwrite = 16
    };

    /**
     * @brief translates the access flags to hdf5 compatible access flags
     * (eg. H5F_ACC_TRUNC).
     * @param mode access mode
     * @return hdf5 access mode
     */
    static uint32_t accessMode(AccessFlag mode);

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
    static bool isValidH5File(QByteArray const& filePath);

    /**
     * @brief hdf5 file suffix not including dot
     * @return file suffix
     */
    static QByteArray fileSuffix();
    /**
     * @brief hdf5 file suffix not including dot
     * @return file suffix
     */
    static QByteArray dotFileSuffix();

    /**
     * @brief GtH5File
     */
    GtH5File();
    GtH5File(QFile const& h5File, AccessFlag flag);
    GtH5File(QString const& path, AccessFlag flag);
    GtH5File(QByteArray const& path, AccessFlag flag);

    /**
     * @brief allows access of the base hdf5 object.
     * @return base hdf5 object
     */
    H5::H5File toH5() const;

    /**
     * @brief id or handle of the hdf5 resource.
     * @return id
     */
    int64_t id() const override;

    /**
     * @brief returns the root group of the file. The resulting object should
     * be valid as long as this object is valid.
     * @return root group of the file
     */
    GtH5Group root() const;

    /**
     * @brief fileName including file suffix.
     * @return filename
     */
    QByteArray fileName() const;

    /**
     * @brief file base name not including suffix.
     * @return filename
     */
    QByteArray fileBaseName() const;

    /**
     * @brief file path used to create this file.
     * @return filepath
     */
    QByteArray const& filePath() const;

    /**
     * @brief explicitly closes the resource handle.
     */
    void close();

private:

    /// hdf5 base instance
    H5::H5File m_file{};
    /// stores the file path used to create this object
    QByteArray m_filePath{};
    /// cashes the associated root group
    mutable GtH5Group m_root{};
};

#endif // GTH5FILE_H
