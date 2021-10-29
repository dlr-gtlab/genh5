/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5ABTSRACTDATASET_H
#define GTH5ABTSRACTDATASET_H

#include "gt_h5_exports.h"
#include "gt_h5datatype.h"
#include "gt_h5dataspace.h"
#include "gt_h5data.h"

#include <QVector>

/**
 * @brief The GtH5AbtsractDataSet class
 */
class GT_H5_EXPORT GtH5AbtsractDataSet
{
public:

    /**
     * @brief The AccessFlag enum
     */
    enum AccessFlag
    {
        Create,             // fails if dataset does exist
        CreateOpen,         // if dataset exists checks for equal dimensions
        CreateOverwrite     // deletes the dataset if its exists
    };

    /**
     * @brief dataType of this dataset
     * @return dataType
     */
    GtH5DataType dataType() const;
    /**
     * @brief dataSpace of this dataset
     * @return dataSpace
     */
    GtH5DataSpace dataSpace() const;

    bool write(void* data) const;
    template<typename T>
    bool write(const QVector<T>& data) const;
    template<typename T>
    bool write(GtH5AbstractData<T>& data) const;

    bool read(void* data) const;
    template<typename T>
    bool read(QVector<T>& data) const;
    template<typename T>
    bool read(GtH5AbstractData<T>& data) const;

protected:

    /**
     * @brief GtH5AbtsractDataSet
     */
    GtH5AbtsractDataSet() {}

    /// datatype of this dataset
    GtH5DataType  m_datatype;
    /// dataspace of this dataset
    GtH5DataSpace m_dataspace;

    virtual bool doWrite(void* data) const = 0;
    virtual bool doRead(void* data) const = 0;
};



template<typename T> bool
GtH5AbtsractDataSet::write(const QVector<T>& data) const
{
    if (data.length() < dataSpace().sum())
    {
        qCritical() << "HDF5: Writing data failed! (to few data elemts)";
        return false;
    }

    return write((void*) data.constData());
}

template<typename T> bool
GtH5AbtsractDataSet::write(GtH5AbstractData<T>& data) const
{
    static_assert (GtH5AbstractData<T>::isImplemented::value,
                   "HDF5: data vector specialization is not implemented!");

    if (data.dataType() != dataType())
    {
        qCritical() << "HDF5: writing data vector failed! "
                       "(datatypes mismatch)";
        return false;
    }

    if (data.dataPtr() == Q_NULLPTR)
    {
        qCritical() << "HDF5: Writing data vector failed! "
                       "(data vector is invalid)";
        return false;
    }

    const int size = dataSpace().sum();

    if (data.length() < size)
    {
        qCritical() << "HDF5: Writing data vector failed! (to few data elemts:"
                    << data.length() << "vs." << size << "elements)";
        return false;
    }

    return write(data.dataPtr());
}

template<typename T> bool
GtH5AbtsractDataSet::read(QVector<T>& data) const
{
    data.clear();
    data.resize(dataSpace().sum());

    return read(data.data());
}

template<typename T> bool
GtH5AbtsractDataSet::read(GtH5AbstractData<T>& data) const
{
    static_assert (GtH5AbstractData<T>::isImplemented::value,
                   "HDF5: data vector specialization is not implemented!");

    if (data.dataType() != dataType())
    {
        qCritical() << "HDF5: Reading data vector failed! "
                       "(datatypes mismatch)";
        return false;
    }

    if (data.dataPtr() == Q_NULLPTR)
    {
        qCritical() << "HDF5: Reading data vector failed! "
                       "(data vector is invalid)";
        return false;
    }

    const int size = dataSpace().sum();

    data.resize(size);

    if (data.length() != size)
    {
        qCritical() << "HDF5: Reading data vector failed! "
                       "(data vector allocation failed:"
                    << data.length() << "vs." << size << "elements)";
        return false;
    }

    return read(data.dataPtr());
}

#endif // GTH5ABTSRACTDATASET_H
