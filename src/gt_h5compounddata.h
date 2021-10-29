/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GT_H5COMPOUNDDATA_H
#define GT_H5COMPOUNDDATA_H

#include "gt_h5data.h"


/// "hides" the helper structs that are used internaly
namespace GtH5T
{
    /**
     * @brief The CompData2D struct
     */
    template<typename T1, typename T2>
    struct CompData2D
    {
        typename GtH5Data<T1>::Data a;
        typename GtH5Data<T2>::Data b;
    };

    /**
     * @brief The CompData3D struct
     */
    template<typename T1, typename T2, typename T3>
    struct CompData3D
    {
        typename GtH5Data<T1>::Data a;
        typename GtH5Data<T2>::Data b;
        typename GtH5Data<T3>::Data c;
    };
}

/* 2D Comp Data */

/**
 * @brief The GtH5Data<T1, T2> struct
 */
template<typename T1, typename T2>
struct GtH5Data<T1, T2> :
        public GtH5AbstractData<GtH5T::CompData2D<T1, T2>>
{
    /// typedef for T
    using Data = GtH5T::CompData2D<T1, T2>;

    /**
     * @brief dataType
     * @return GtH5DataType
     */
    GtH5DataType dataType() const override;

    /**
     * @brief GtH5Data
     * @param IN: t1
     * @param IN: t2
     */
    template<typename T1vec, typename T2vec>
    GtH5Data(const T1vec& t1, const T2vec& t2);
    GtH5Data() = default;

    /**
     * @brief  deserializes the internal buffer
     * @param OUT: t1
     * @param OUT: t2
     */
    template<typename T1vec, typename T2vec>
    void deserialize(T1vec& t1, T2vec& t2);

private:

    /// internal buffer for T1
    GtH5Data<T1> m_buffer1;
    /// internal buffer for T2
    GtH5Data<T2> m_buffer2;
};

/* 3D Comp Data */

/**
 * @brief The GtH5Data<T1, T2, T3> struct
 */
template<typename T1, typename T2, typename T3>
struct GtH5Data<T1, T2, T3> :
        public GtH5AbstractData<GtH5T::CompData3D<T1, T2, T3>>
{
    /// typedef for T
    using Data = GtH5T::CompData3D<T1, T2, T3>;

    /**
     * @brief dataType
     * @return GtH5DataType
     */
    GtH5DataType dataType() const override;

    /**
     * @brief GtH5Data
     * @param IN: t1
     * @param IN: t2
     * @param IN: t3
     */
    template<typename T1vec, typename T2vec, typename T3vec>
    GtH5Data(const T1vec& t1, const T2vec& t2, const T3vec& t3);
    GtH5Data() = default;

    /**
     * @brief  deserializes the internal buffer
     * @param OUT: t1
     * @param OUT: t2
     * @param OUT: t3
     */
    template<typename T1vec, typename T2vec, typename T3vec>
    void deserialize(T1vec& t1, T2vec& t2, T3vec& t3);

private:

    /// internal buffer for T1
    GtH5Data<T1> m_buffer1;
    /// internal buffer for T2
    GtH5Data<T2> m_buffer2;
    /// internal buffer for T3
    GtH5Data<T3> m_buffer3;
};


/* Implementations */
template<typename T1, typename T2>
GtH5DataType GtH5Data<T1, T2>::dataType() const
{
    H5::CompType dtype (sizeof (Data));
    dtype.insertMember("type 1", offsetof(Data, a),
                       m_buffer1.dataType().toH5());
    dtype.insertMember("type 2", offsetof(Data, b),
                       m_buffer2.dataType().toH5());
    return dtype;
}

template<typename T1, typename T2>
template<typename T1vec, typename T2vec>
GtH5Data<T1, T2>::GtH5Data(const T1vec& t1,
                           const T2vec& t2) :
    m_buffer1(t1),
    m_buffer2(t2)
{
    const int length = t1.length();
    if (length != t2.length())
    {
        qCritical() << "HDF5: Instantiating data vector failed! "
                       "(unequal number of elements in t1, and t2)";
        return;
    }

    auto& buffer1 = m_buffer1.data();
    auto& buffer2 = m_buffer2.data();

    this->m_data.clear();
    this->m_data.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        this->m_data.append({ buffer1.at(i), buffer2.at(i) });
    }
}

template<typename T1, typename T2>
template<typename T1vec, typename T2vec>
void
GtH5Data<T1, T2>::deserialize(T1vec& t1,
                              T2vec& t2)
{
    const int length(this->m_data.length());
    t1.clear();
    t2.clear();
    t1.reserve(length);
    t2.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        const Data& data(this->m_data.at(i));
        t1.append(data.a);
        t2.append(data.b);
    }
}

template<typename T1, typename T2, typename T3>
GtH5DataType GtH5Data<T1, T2, T3>::dataType() const
{
    H5::CompType dtype (sizeof (Data));
    dtype.insertMember("type 1", offsetof(Data, a),
                       m_buffer1.dataType().toH5());
    dtype.insertMember("type 2", offsetof(Data, b),
                       m_buffer2.dataType().toH5());
    dtype.insertMember("type 3", offsetof(Data, c),
                       m_buffer3.dataType().toH5());
    return dtype;
}

template<typename T1, typename T2, typename T3>
template<typename T1vec, typename T2vec, typename T3vec>
GtH5Data<T1, T2, T3>::GtH5Data(const T1vec& t1,
                               const T2vec& t2,
                               const T3vec& t3) :
    m_buffer1(t1),
    m_buffer2(t2),
    m_buffer3(t3)
{
    const int length = t1.length();
    if (length != t2.length() || length != t3.length())
    {
        qCritical() << "HDF5: Instantiating data vector failed! "
                       "(unequal number of elements in t1, t2 and t3)";
        return;
    }

    auto& buffer1 = m_buffer1.data();
    auto& buffer2 = m_buffer2.data();
    auto& buffer3 = m_buffer3.data();

    this->m_data.clear();
    this->m_data.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        this->m_data.append({ buffer1.at(i), buffer2.at(i), buffer3.at(i)});
    }
}

template<typename T1, typename T2, typename T3>
template<typename T1vec, typename T2vec, typename T3vec>
void
GtH5Data<T1, T2, T3>::deserialize(T1vec& t1,
                                  T2vec& t2,
                                  T3vec& t3)
{
    const int length(this->m_data.length());
    t1.clear();
    t3.clear();
    t2.clear();
    t1.reserve(length);
    t2.reserve(length);
    t3.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        const Data& data(this->m_data.at(i));
        t1.append(data.a);
        t2.append(data.b);
        t3.append(data.c);
    }
}

#endif // GT_H5COMPOUNDDATA_H
