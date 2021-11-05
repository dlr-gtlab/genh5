/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GT_H5DABSTRACTDATA_H
#define GT_H5DABSTRACTDATA_H

#include "gt_h5datatype.h"
#include <QByteArray>
#include <QVector>

/**
 * @brief The GtH5AbstractData struct
 */
template<typename T>
struct GtH5AbstractData
{
    /// typedef for T
    using Data = T;

    /**
     * @brief The isImplemented struct helper struct to check whether a datatype
     * has a valid implementation
     */
    struct isImplemented :
            std::integral_constant<bool, !std::is_empty<T>::value> {};

    /**
     * @brief dataType
     * @return GtH5DataType
     */
    virtual GtH5DataType dataType() const = 0;

    /**
     * @brief reference to data array
     * @return reference to data array
     */
    virtual inline QVector<T>& data() { return m_data; }

    /**
     * @brief dataPtr returns pointer to first element of data array. can be
     * used to write and read data from a dataset
     * @return pointer to data array
     */
    inline void* dataPtr() { return data().data(); }

    /**
     * @brief isEmpty
     * @return
     */
    inline bool isEmpty() { return data().isEmpty(); }

    /**
     * @brief length
     * @return
     */
    inline int length() { return data().length(); }

    /**
     * @brief resize used to pre allocate the data array. This is used for
     * reading data from a dataset
     * @param length length
     */
    inline void resize(const int length)
    {
        data().clear();
        data().resize(length);
    }

    /**
     * @brief reseve
     * @param length
     */
    inline void reserve(const int length) { data().reserve(length); }

    /**
     * @brief append
     * @param data
     */
    inline void append(const T& d) { data().append(d); }
    inline void append(const QVector<T>& d) { data().append(d); }

    /**
     * @brief clear
     */
    inline void clear() { data().clear(); }

    // operators
    operator QVector<T>&() { return data(); }
    operator GtH5DataType() { return dataType(); }

protected:

    /**
     * @brief GtH5AbstractData
     */
    GtH5AbstractData() {}

    /// internal data array
    QVector<T> m_data;
};

#endif // GT_H5DABSTRACTDATA_H
