/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATA_SIMPLE_H
#define GTH5_DATA_SIMPLE_H

#include "gth5_abstractdata.h"
#include <QDebug>

#define GTH5_DECLARE_SIMPLE_DATATYPE(NATIVE_TYPE, HDF5_TYPE) \
    template<>\
    struct GtH5Data<NATIVE_TYPE> : GtH5AbstractData<NATIVE_TYPE> {\
      GtH5DataType dataType() const override {\
        return GtH5DataType(H5::PredType::HDF5_TYPE); \
      }\
      GtH5Data() = default; \
      GtH5Data(QVector<Data> const& data) : GtH5AbstractData{data} {} \
      void deserialize(QVector<Data>& data) { data = m_data; } \
    };

/// "hides" the datatype helper structs that are used internaly
namespace GtH5T
{
    /**
     * @brief empty struct to "disable" certain data types that are not
     * implementd yet. Used internally.
     */
    struct InvalidType {};
}

/**
 * @brief The GtH5Data struct. This specialisation is invalid.
 */
template<typename ...T>
struct GtH5Data : GtH5AbstractData<GtH5T::InvalidType>
{
    /**
     * @brief returns the datatype associated with the template arguments. This
     * datatype is invalid.
     * @return datatype
     */
    GtH5DataType dataType() const { return GtH5DataType(); }
};


/* Primtiive Datatypes */
GTH5_DECLARE_SIMPLE_DATATYPE(int, NATIVE_INT);
GTH5_DECLARE_SIMPLE_DATATYPE(long, NATIVE_LONG);
GTH5_DECLARE_SIMPLE_DATATYPE(float, NATIVE_FLOAT);
GTH5_DECLARE_SIMPLE_DATATYPE(double, NATIVE_DOUBLE);
GTH5_DECLARE_SIMPLE_DATATYPE(const char*, C_S1);


/* String Datatypes */

/**
 * @brief The GtH5Data<QByteArray> struct
 */
template<>
struct GtH5Data<QByteArray> : GtH5AbstractData<const char*>
{    
    /**
     * @brief GtH5Data
     * @param IN: data
     */
    GtH5Data(QVector<QByteArray> const& data);
    GtH5Data() = default;

    /**
     * @brief returns the datatype associated with the template arguments.
     * @return datatype
     */
    GtH5DataType dataType() const override;

    /**
     * @brief deserializes the internal char buffer to QByteArrays
     * @param OUT: data
     */
    void deserialize(QVector<QByteArray>& data);

private:

    /// internal buffer to keep const char* valid
    QVector<QByteArray> m_buffer{};
};

/**
 * @brief The GtH5Data<QString> struct
 */
template<>
struct GtH5Data<QString> : GtH5AbstractData<const char*>
{
    /**
     * @brief returns the datatype associated with the template arguments.
     * @return datatype
     */
    GtH5DataType dataType() const override;

    /**
     * @brief GtH5Data
     * @param IN: data
     */
    GtH5Data(QStringList const& data);
    GtH5Data() = default;

    /**
     * @brief deserializes the internal buffer to QStrings
     * @param OUT: data
     */
    void deserialize(QStringList& data);

private:

    /// internal buffer to keep const char* valid
    QVector<QByteArray> m_buffer{};
};


/* Implementations */
inline GtH5Data<QByteArray>::GtH5Data(QVector<QByteArray> const& data) :
    m_buffer(data)
{
    const int length = data.length();
    reserve(length);
    for (int i = 0; i < length; ++i)
    {
        m_data.append(m_buffer.at(i).constData());
    }
}

inline GtH5DataType
GtH5Data<QByteArray>::dataType() const
{
    return GtH5DataType::VarString;
}

inline void
GtH5Data<QByteArray>::deserialize(QVector<QByteArray>& data)
{
    data.clear();
    data.reserve(length());
    for (const auto& d : qAsConst(m_data))
    {
        data.append(d);
    }
}

inline GtH5Data<QString>::GtH5Data(QStringList const& data)
{
    const int length = data.length();
    reserve(length);
    m_buffer.reserve(length);
    for (int i = 0; i < length; ++i)
    {
        m_buffer.append(data.at(i).toUtf8());
        m_data.append(m_buffer.at(i).constData());
    }
}

inline GtH5DataType
GtH5Data<QString>::dataType() const
{
    return GtH5DataType::VarString;
}

inline void
GtH5Data<QString>::deserialize(QStringList& data)
{
    data.clear();
    data.reserve(length());
    for (auto const& d : qAsConst(m_buffer))
    {
        data.append(d);
    }
}

#endif // GTH5_DATA_SIMPLE_H
