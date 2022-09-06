
#include "gtest/gtest.h"
#include "testhelper.h"

#include <QDebug>
#include <QPoint>
#include <QPointF>

#include "genh5.h"
#include "genh5_data.h"

class Tests : public testing::Test
{

};

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

