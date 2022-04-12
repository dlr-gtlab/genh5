/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.10.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DABSTRACTDATA_H
#define GTH5_DABSTRACTDATA_H

#include "gth5_datatype.h"
#include "gth5_dataspace.h"
#include <QByteArray>
#include <QVector>

/**
 * @brief The GtH5AbstractData struct
 */
template<typename Tdata>
struct GtH5AbstractData
{
    virtual ~GtH5AbstractData() = default;

    /// typedef for T
    using Data = Tdata;

    /**
     * @brief helper struct to check at build time whether this datatype has a
     * valid implementation
     */
    struct isImplemented :
            std::integral_constant<bool, !std::is_empty<Data>::value> {};

    /**
     * @brief dataType
     * @return GtH5DataType
     */
    virtual GtH5DataType dataType() const = 0;

    GtH5DataSpace dataSpace() const {
        return GtH5DataSpace{static_cast<uint64_t>(m_data.length())};
    }

    /**
     * @brief reference to data array
     * @return reference to data array
     */
    /**@{*/
    QVector<Data> const& data() const { return m_data; }
    QVector<Data>& data() { return m_data; }
    /**@}*/

    /**
     * @brief Returns pointer to first element of data array. Can be
     * used to write and read data from a dataset
     * @return pointer to data array
     */
    /**@{*/
    void const* dataPtr() const { return data().constData(); }
    void* dataPtr() { return data().data(); }
    /**@}*/

    /**
     * @brief isEmpty
     * @return
     */
    inline bool isEmpty() const { return data().isEmpty(); }

    /**
     * @brief length
     * @return
     */
    inline int length() const { return data().length(); }

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
     * @param length length
     */
    inline void reserve(const int length) { data().reserve(length); }

    /**
     * @brief append
     * @param d data
     */
     /**@{*/
    inline void append(const Data& d) { data().append(d); }
    inline void append(const QVector<Data>& d) { data().append(d); }
    /**@}*/

    /**
     * @brief clear
     */
    inline void clear() { data().clear(); }

    /**
     * @brief operator QVector<T> &
     */
    operator QVector<Data>&() { return data(); }

protected:

    /**
     * @brief GtH5AbstractData
     */
    GtH5AbstractData() = default;
    GtH5AbstractData(const QVector<Data>& data) : m_data{data} {}

    GtH5AbstractData(GtH5AbstractData const& other) = default;
    GtH5AbstractData(GtH5AbstractData&& other) = default;
    GtH5AbstractData& operator=(GtH5AbstractData const& other) = default;
    GtH5AbstractData& operator=(GtH5AbstractData&& other) = default;

    /// internal data array
    QVector<Data> m_data{};
};

#endif // GTH5_DABSTRACTDATA_H
