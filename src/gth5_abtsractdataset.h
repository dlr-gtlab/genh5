/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5ABTSRACTDATASET_H
#define GTH5ABTSRACTDATASET_H

#include "gth5_exports.h"
#include "gth5_datatype.h"
#include "gth5_dataspace.h"
#include "gth5_abstractdata.h"

#include <QVector>
#include <QDebug>

/**
 * @brief The GtH5AbtsractDataSet class
 */
class GTH5_EXPORT GtH5AbtsractDataSet
{
public:
    virtual ~GtH5AbtsractDataSet() = default;

    /**
     * @brief dataType of this dataset
     * @return dataType
     */
    GtH5DataType const& dataType() const;
    /**
     * @brief dataSpace of this dataset
     * @return dataSpace
     */
    GtH5DataSpace const& dataSpace() const;

    bool write(void const* data) const;
    template<typename T>
    bool write(QVector<T> const& data) const;
    template<typename T>
    bool write(GtH5AbstractData<T> const& data) const;

    bool read(void* data) const;
    template<typename T>
    bool read(QVector<T>& data) const;
    template<typename T>
    bool read(GtH5AbstractData<T>& data) const;

protected:

    /**
     * @brief GtH5AbtsractDataSet
     */
    GtH5AbtsractDataSet(GtH5DataType const& dtype = {},
                        GtH5DataSpace const& dspace = {});

    GtH5AbtsractDataSet(GtH5AbtsractDataSet const& other) = default;
    GtH5AbtsractDataSet(GtH5AbtsractDataSet&& other) = default;
    GtH5AbtsractDataSet& operator=(GtH5AbtsractDataSet const& other) = default;
    GtH5AbtsractDataSet& operator=(GtH5AbtsractDataSet&& other) = default;

    /// datatype of this dataset
    GtH5DataType  m_datatype{};
    /// dataspace of this dataset
    GtH5DataSpace m_dataspace{};

    virtual bool doWrite(void const* data) const = 0;
    virtual bool doRead(void* data) const = 0;
};



template<typename T> bool
GtH5AbtsractDataSet::write(QVector<T> const& data) const
{
    if (data.length() < dataSpace().sum())
    {
        qCritical() << "HDF5: Writing data failed!" <<
                       "(To few data elements)";
        return false;
    }

    return write(static_cast<const void*>(data.data()));
}

template<typename T> bool
GtH5AbtsractDataSet::write(GtH5AbstractData<T> const& data) const
{
    static_assert (GtH5AbstractData<T>::isImplemented::value,
                   "HDF5: Datavector specialization is not implemented!");

    if (data.dataType() != dataType())
    {
        qCritical() << "HDF5: Writing datavector failed!"
                    << "(Datatype mismatch)";
        return false;
    }

    if (data.dataPtr() == nullptr)
    {
        qCritical() << "HDF5: Writing datavector failed!"
                    << "(Datavector is invalid)";
        return false;
    }

    const int size = dataSpace().sum();

    if (data.length() < size)
    {
        qCritical() << "HDF5: Writing datavector failed"
                    << "(To few data elements for dataspace:"
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
                   "HDF5: Datavector specialization is not implemented!");

    if (data.dataType() != dataType())
    {
        qCritical() << "HDF5: Reading datavector failed!"
                    << "(Datatype mismatch)";
        return false;
    }

    if (data.dataPtr() == nullptr)
    {
        qCritical() << "HDF5: Reading datavector failed!"
                    << "(Datavector is invalid)";
        return false;
    }

    const int size = dataSpace().sum();

    data.resize(size);

    if (data.length() != size)
    {
        qCritical() << "HDF5: Reading datavector failed!"
                    << "(Datavector allocation failed:"
                    << data.length() << "vs." << size << "elements)";
        return false;
    }

    return read(data.dataPtr());
}

#endif // GTH5ABTSRACTDATASET_H
