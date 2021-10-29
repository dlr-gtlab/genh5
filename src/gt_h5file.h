/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5FILE_H
#define GTH5FILE_H

#include "gt_h5object.h"
#include "gt_h5group.h"

#include <QByteArray>

class QFile;

/**
 * @brief The GtH5File class
 */
class GT_H5_EXPORT GtH5File : public GtH5Object
{
public:

    /**
     * @brief The AccessFlag enum
     */
    enum AccessFlag
    {
        OpenReadOnly,
        OpenReadWrite,
        CreateReadWrite,
        CreateNotExisting,
        CreateOverwrite
    };

    /**
     * @brief translates the access flags to hdf5 compatible access flags
     * (eg. H5F_ACC_TRUNC).
     * @param mode
     * @return hdf5 access mode
     */
    static uint accessMode(AccessFlag mode);

    /**
     * @brief returns whether the file exists
     * @param file path
     * @return whether the file exists
     */
    static bool fileExists(const QString& path);

    /**
     * @brief returns whether the file is a valid hdf5 file. Does not check if
     * file is corrupted
     * @param file path
     * @return whether the file is a valid hdf5 file
     */
    static bool isValidH5File(const QString& filePath);
    static bool isValidH5File(const QByteArray& filePath);

    /**
     * @brief hdf5 file suffix including dot
     * @return hdf5 file suffix including dot
     */
    static QByteArray fileSuffix();

    /**
     * @brief GtH5File
     */
    GtH5File() {}
    GtH5File(const QFile& h5File, AccessFlag flag);
    GtH5File(const QString& path, AccessFlag flag);
    GtH5File(const QByteArray& path, AccessFlag flag);

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
    QByteArray filePath() const;

    /**
     * @brief explicitly closes the resource handle.
     */
    void close();

private:

    /// hdf5 base instance
    H5::H5File m_file;
    /// stores the file path used to create this object
    QByteArray m_filePath;
    /// cashes the associated root group
    mutable GtH5Group m_root;
};

#endif // GTH5FILE_H
