/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATA_COMP_H
#define GTH5_DATA_COMP_H

#include "gth5_data_simple.h"

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
     * @brief helper struct to check at build time whether this datatype has a
    /// valid implementation
     */
    struct isImplemented :
            std::integral_constant<bool,
                                   GtH5Data<T1>::isImplemented::value &&
                                   GtH5Data<T2>::isImplemented::value> {};
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
    explicit GtH5Data(T1vec const& t1, T2vec const& t2);
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
    GtH5Data<T1> m_buffer1{};
    /// internal buffer for T2
    GtH5Data<T2> m_buffer2{};
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
     * @brief helper struct to check at build time whether this datatype has a
    /// valid implementation
     */
    struct isImplemented :
            std::integral_constant<bool,
                                   GtH5Data<T1>::isImplemented::value &&
                                   GtH5Data<T2>::isImplemented::value &&
                                   GtH5Data<T3>::isImplemented::value> {};

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
    explicit GtH5Data(T1vec const& t1, T2vec const& t2, T3vec const& t3);
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
    GtH5Data<T1> m_buffer1{};
    /// internal buffer for T2
    GtH5Data<T2> m_buffer2{};
    /// internal buffer for T3
    GtH5Data<T3> m_buffer3{};
};


/* Implementations */
template<typename T1, typename T2>
GtH5DataType GtH5Data<T1, T2>::dataType() const
{
    H5::CompType dtype (sizeof (Data));

    herr_t err;
    err  = H5Tinsert(dtype.getId(), "type 1", offsetof(Data, a),
                     m_buffer1.dataType().id());
    err |= H5Tinsert(dtype.getId(), "type 2", offsetof(Data, b),
                     m_buffer2.dataType().id());
    if (err < 0)
    {
        return GtH5DataType();
    }
    return GtH5DataType(dtype);
}

template<typename T1, typename T2>
template<typename T1vec, typename T2vec>
GtH5Data<T1, T2>::GtH5Data(T1vec const& t1,
                           T2vec const& t2) :
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

    const auto& buffer1 = m_buffer1.data();
    const auto& buffer2 = m_buffer2.data();

    this->m_data.clear();
    this->m_data.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        this->m_data.append(Data{ buffer1.at(i), buffer2.at(i) });
    }
}

template<typename T1, typename T2>
template<typename T1vec, typename T2vec>
void
GtH5Data<T1, T2>::deserialize(T1vec& t1,
                              T2vec& t2)
{
    const int length{this->m_data.length()};
    t1.clear();
    t2.clear();
    t1.reserve(length);
    t2.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        Data const& data{this->m_data.at(i)};
        t1.append(data.a);
        t2.append(data.b);
    }
}

template<typename T1, typename T2, typename T3>
GtH5DataType GtH5Data<T1, T2, T3>::dataType() const
{
    H5::CompType dtype (sizeof (Data));

    herr_t err;
    err  = H5Tinsert(dtype.getId(), "type 1", offsetof(Data, a),
              m_buffer1.dataType().id());
    err |= H5Tinsert(dtype.getId(), "type 2", offsetof(Data, b),
              m_buffer2.dataType().id());
    err |= H5Tinsert(dtype.getId(), "type 3", offsetof(Data, c),
              m_buffer3.dataType().id());
    if (err < 0)
    {
        return GtH5DataType();
    }
    return GtH5DataType(dtype);
}

template<typename T1, typename T2, typename T3>
template<typename T1vec, typename T2vec, typename T3vec>
GtH5Data<T1, T2, T3>::GtH5Data(T1vec const& t1,
                               T2vec const& t2,
                               T3vec const& t3) :
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

    const auto& buffer1 = m_buffer1.data();
    const auto& buffer2 = m_buffer2.data();
    const auto& buffer3 = m_buffer3.data();

    this->m_data.clear();
    this->m_data.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        this->m_data.append(Data{ buffer1.at(i), buffer2.at(i), buffer3.at(i)});
    }
}

template<typename T1, typename T2, typename T3>
template<typename T1vec, typename T2vec, typename T3vec>
void
GtH5Data<T1, T2, T3>::deserialize(T1vec& t1,
                                  T2vec& t2,
                                  T3vec& t3)
{
    const int length{this->m_data.length()};
    t1.clear();
    t3.clear();
    t2.clear();
    t1.reserve(length);
    t2.reserve(length);
    t3.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        Data const& data{this->m_data.at(i)};
        t1.append(data.a);
        t2.append(data.b);
        t3.append(data.c);
    }
}

#endif // GTH5_DATA_COMP_H
