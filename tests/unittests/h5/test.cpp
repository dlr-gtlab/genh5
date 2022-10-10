
#include "gtest/gtest.h"
#include "testhelper.h"

#include <QDebug>
#include <QPoint>
#include <QPointF>

#include "genh5.h"

class Tests : public testing::Test
{

};

TEST_F(Tests, filterDeflate)
{
    bool isAvail = H5Zfilter_avail(H5Z_FILTER_DEFLATE);
    qDebug() << "H5Zfilter_avail(H5Z_FILTER_DEFLATE):" << isAvail;
#if 0
    EXPECT_TRUE(isAvail);
#endif
}

#if 0
TEST_F(Tests, test)
{
    H5::DSetCreatPropList props;
    props.setDeflate(9);

    uint flags{};       // 0 = mandatory, 1 = optional in pipeline
    size_t len{1};      // length of the filter data, for "deflate" we require only one entry
    uint compression{}; // "compression level" is the first entry of the user data
    char name[30];      // name of filter
    uint config{};      // filter config - not relevant?

    int nfilter = H5Pget_nfilters(props.getId());

    qDebug() << "Number of filters:" << nfilter;

    herr_t err = H5Pget_filter_by_id(props.getId(), H5Z_FILTER_DEFLATE, &flags,
                                     &len, &compression, 30, name, &config);

    qDebug() << "Error:" << err;
    qDebug() << "Is Optional:" << flags;
    qDebug() << "Compression:" << compression;
    qDebug() << "Filter Name:" << name;
    qDebug() << "Config:" << config;
}
#endif


//template<typename T>
//struct VarLen : public Vector<T>
//{
//    using base_class = Vector<T>;
//    using base_class::base_class;
////    using base_class::operator=;
//    using base_class::operator+;
//    using base_class::operator[];

//    using base_class::operator!=;
//    using base_class::operator==;

//    VarLen() = default;
//    template <typename U>
//    VarLen(U&& u) : base_class{std::forward<U>(u)} {}

//    template <typename U>
//    VarLen& operator=(U&& u) {
//        VarLen temp(std::forward<U>(u));
//        swap(temp);
//        return *this;
//    }
//};


//struct FixedString : public QByteArray
//{
//    using QByteArray::QByteArray;
//    using QByteArray::operator=;
//    using QByteArray::operator+=;

////    using QByteArray::operator<;
////    using QByteArray::operator<=;
////    using QByteArray::operator==;
////    using QByteArray::operator!=;
////    using QByteArray::operator>=;
////    using QByteArray::operator>;

////    using QByteArray::operator const char *;
////    using QByteArray::operator const void *;
//};

//GENH5_DECLARE_CONVERSION_TYPE(GenH5::FixedString, char*);
//GENH5_DECLARE_BUFFER_TYPE(GenH5::FixedString, QByteArray);
//GENH5_DECLARE_DATATYPE(GenH5::FixedString, DataType::VarString);

//template<>
//struct conversion_container<FixedString>
//{
//    using type = FixedString;
//};

