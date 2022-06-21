/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 23.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "testhelper.h"
#include "gth5.h"

#include "gth5_conversion.h"

#include <QPoint>

class TestConversion : public testing::Test
{

};

// QPOINT
struct PointData{ int xp, yp; };

GTH5_DECLARE_CONVERSION_TYPE(QPoint, PointData);

GTH5_DECLARE_CONVERSION(QPoint, value, /*buffer*/)
{
    return PointData{value.x(), value.y()};
}

GTH5_DECLARE_REVERSE_CONVERSION(QPoint, PointData, value)
{
    return QPoint{value.xp, value.yp};
}

TEST_F(TestConversion, convertPod)
{
    using GtH5::convert;

    auto c0 = "hello world 1";
    auto c1 = convert("hello world 1");

    EXPECT_STREQ(c0, c1);

    EXPECT_EQ(convert(42.0), 42);
    EXPECT_EQ(convert(42.1f), 42.1f);
    EXPECT_EQ(convert(true), true);
}

TEST_F(TestConversion, convertStrings)
{
    using GtH5::convert;
    using GtH5::convertTo;

    GtH5::Vector<QByteArray> buffer;

    auto c0 = "hello world";
    auto c1 = convert(QByteArray{"hello world"}, buffer);
    EXPECT_EQ(buffer.length(), 1);
    EXPECT_STREQ(c1, c0);

    auto c2 = convert(QString{"hello world"}, buffer);
    EXPECT_EQ(buffer.length(), 2);
    EXPECT_STREQ(c2, c0);

    // reverse conversion
    EXPECT_EQ(QByteArray{c1}, convertTo<QByteArray>(c1));
    EXPECT_EQ(QString{c2}, convertTo<QString>(c2));
}

TEST_F(TestConversion, convertQPoint)
{
    using GtH5::convert;
    using GtH5::convertTo;

    QPoint pointOrig{1, 2};
    GtH5::buffer_t<QPoint> buffer;
    auto res = convert(pointOrig, buffer);

    static_assert (std::is_same<decltype(res), PointData>::value,
                   "Conversion type mismatch!");

    auto point = convertTo<QPoint>(res);
    EXPECT_EQ(point, pointOrig);
}

TEST_F(TestConversion, convertArray)
{
    GtH5::Array<QByteArray, 4> arrayOrig{"Hello World", "ABC",
                                         "Fancy String", "DEF"};
    GtH5::buffer_t<decltype(arrayOrig)> buffer;

    auto x = buffer;

    auto res = GtH5::convert(arrayOrig, buffer);

    EXPECT_EQ(res.size(), arrayOrig.size());
    EXPECT_EQ(buffer.size(), arrayOrig.size());

    auto array = GtH5::convertTo<decltype(arrayOrig)>(res);
    EXPECT_EQ(array, arrayOrig);
}

TEST_F(TestConversion, convertArrayComp)
{
    using CompT = GtH5::Comp<QByteArray, double>;
    using ArrayT = GtH5::Array<CompT, 4>;

    ArrayT arrayOrig{
        CompT{"QPoint{1,1}", 2},
        CompT{"QPoint{1,2}", 42.1},
        CompT{"QPoint{2,0}", 0.12},
        CompT{"{}", .1}
    };

    GtH5::buffer_t<decltype(arrayOrig)> buffer;

    auto res = GtH5::convert(arrayOrig, buffer);

    static_assert (std::is_same<decltype (res), GtH5::conversion_t<ArrayT>
                   >::value, "Conversion type mismatch!");

    auto array = GtH5::convertTo<ArrayT>(res);

    static_assert (std::is_same<decltype (array), ArrayT
                   >::value, "Conversion type mismatch!");

    EXPECT_TRUE(std::equal(std::cbegin(array), std::cend(array),
                           std::cbegin(arrayOrig)));
}

TEST_F(TestConversion, convertVarLen)
{
    GtH5::VarLen<QString> varlenOrig{"Hello World", "ABC",
                                     "Fancy String", "DEF"};
    GtH5::buffer_t<decltype(varlenOrig)> buffer;

    auto res = GtH5::convert(varlenOrig, buffer);

    static_assert (std::is_same<hvl_t, decltype (res)>::value,
                   "Conversion type mismatch");

    ASSERT_EQ(buffer.size(), 1);
    auto& first = buffer.first();
    EXPECT_EQ(res.p, first.data.constData());
    EXPECT_EQ(res.len, first.data.size());
    EXPECT_EQ(res.len, varlenOrig.size());
    EXPECT_EQ(first.buffer.size(), varlenOrig.size());

    auto varlen = GtH5::convertTo<GtH5::VarLen<QString>>(res);

    EXPECT_EQ(varlen, varlenOrig);
}

TEST_F(TestConversion, convertVarLenQPoint)
{
    GtH5::VarLen<QPoint> varlenOrig{{0, 1}, {1, 1}, {1, 2}};
    GtH5::buffer_t<decltype(varlenOrig)> buffer;

    auto res = GtH5::convert(varlenOrig, buffer);

    static_assert (std::is_same<hvl_t, decltype (res)>::value,
                   "Conversion type mismatch");

    ASSERT_EQ(buffer.size(), 1);
    auto& first = buffer.first();
    EXPECT_EQ(res.p, first.data.constData());
    EXPECT_EQ(res.len, first.data.size());
    EXPECT_EQ(res.len, varlenOrig.size());

    EXPECT_EQ(first.buffer.size(), 0);

    auto varlen = GtH5::convertTo<GtH5::VarLen<QPoint>>(res);
    EXPECT_EQ(varlen, varlenOrig);
}

TEST_F(TestConversion, convertCompound)
{
    using Tuple = GtH5::Comp<QString, size_t, double>;
    Tuple tuple{"Hello World", 42, 0.12};

    GtH5::buffer_t<Tuple> buffer;

    auto res = GtH5::convert(tuple, buffer);

    EXPECT_STREQ(std::get<2>(res), "Hello World");
    EXPECT_EQ(std::get<1>(res), 42);
    EXPECT_EQ(std::get<0>(res), 0.12);

    EXPECT_EQ(std::get<2>(buffer).size(), 1);
    EXPECT_EQ(std::get<1>(buffer).size(), 0);
    EXPECT_EQ(std::get<0>(buffer).size(), 0);
}

TEST_F(TestConversion, converionTypeSimple)
{
    static_assert (std::is_same<PointData, GtH5::conversion_t<QPoint>>::value,
                   "Conversion type mismatch!");
    static_assert (std::is_same<int, GtH5::conversion_t<int>>::value,
                   "Conversion type mismatch!");
    static_assert (std::is_same<char*, GtH5::conversion_t<QString>>::value,
                   "Conversion type mismatch!");
}

TEST_F(TestConversion, converionTypeArray)
{
    static_assert (std::is_same<GtH5::Array<char*, 42>,
                                GtH5::conversion_t<GtH5::Array<QString, 42>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<GtH5::Array<double, 11>,
                                GtH5::conversion_t<GtH5::Array<double, 11>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<GtH5::Array<hvl_t, 12>,
                                GtH5::conversion_t<
                                    GtH5::Array<GtH5::VarLen<double>, 12>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<GtH5::Array<GtH5::Comp<float, char*>, 2>,
                                GtH5::conversion_t<
                                    GtH5::Array<GtH5::Comp<QString, float>, 2>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<GtH5::Array<GtH5::Comp<double, char*>, 4>,
                                GtH5::conversion_t<
                                    GtH5::Array<GtH5::Comp<QString, double>, 4>>
                   >::value, "Conversion type mismatch!");
}

TEST_F(TestConversion, converionTypeVarLen)
{
    static_assert (std::is_same<hvl_t, GtH5::conversion_t<GtH5::VarLen<QString>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<hvl_t, GtH5::conversion_t<GtH5::VarLen<int>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<hvl_t, GtH5::conversion_t<GtH5::VarLen<double>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<hvl_t, GtH5::conversion_t<
                                    GtH5::VarLen<GtH5::Comp<double, QPoint>>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<hvl_t, GtH5::conversion_t<
                                    GtH5::VarLen<GtH5::Array<double, 42>>>
                   >::value, "Conversion type mismatch!");
}

TEST_F(TestConversion, converionTypeCompound)
{
    static_assert (std::is_same<
                        GtH5::Comp<int, char*>,
                        GtH5::conversion_t<GtH5::Comp<QString, int>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<
                        GtH5::Comp<double, char>,
                        GtH5::conversion_t<GtH5::Comp<char, double>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<
                        GtH5::Comp<size_t, char*, PointData>,
                        GtH5::conversion_t<GtH5::Comp<QPoint, QString, size_t>>
                   >::value, "Conversion type mismatch!");
    static_assert (std::is_same<
                        GtH5::Comp<hsize_t, hvl_t>,
                        GtH5::conversion_t<
                                    GtH5::Comp<GtH5::VarLen<QPoint>, hsize_t>>
                   >::value, "Conversion type mismatch!");
}

TEST_F(TestConversion, bufferTypeSimple)
{
    static_assert (std::is_same<GtH5::buffer_element_t<QString>, QByteArray
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<GtH5::buffer_element_t<double>, double
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<GtH5::buffer_element_t<char*>, char*
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<GtH5::buffer_element_t<QPoint>, QPoint
                   >::value, "Bufer element type mismatch!");
}

TEST_F(TestConversion, bufferTypeArray)
{
    static_assert (std::is_same<
                        double,
                        GtH5::buffer_element_t<GtH5::Array<double, 10>>
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<
                        QByteArray,
                        GtH5::buffer_element_t<GtH5::Array<QByteArray, 42>>
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<
                        GtH5::Comp<GtH5::Vector<QPoint>,
                                   GtH5::Vector<QByteArray>>,
                        GtH5::buffer_element_t<GtH5::Array<
                                    GtH5::Comp<QString, QPoint>, 42>>
                   >::value, "Bufer element type mismatch!");
}

TEST_F(TestConversion, bufferTypeVarLen)
{
    static_assert (std::is_same<
                        GtH5::buffer_element_t<GtH5::VarLen<QString>>,
                        GtH5::details::hvl_buffer<QString>
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<
                        GtH5::buffer_t<GtH5::VarLen<QPoint>>,
                        GtH5::Vector<GtH5::details::hvl_buffer<QPoint>>
                   >::value, "Bufer type mismatch!");
    static_assert (std::is_same<
                        GtH5::buffer_t<GtH5::VarLen<GtH5::Comp<hsize_t, char>>>,
                        GtH5::Vector<
                        GtH5::details::hvl_buffer<GtH5::Comp<hsize_t, char>>>
                   >::value, "Bufer type mismatch!");
}

TEST_F(TestConversion, bufferTypeCompound)
{
    static_assert (std::is_same<
                        GtH5::buffer_element_t<GtH5::Comp<QString>>,
                        GtH5::Comp<GtH5::Vector<QByteArray>>
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<
                        GtH5::buffer_element_t<GtH5::Comp<int, double>>,
                        GtH5::Comp<GtH5::Vector<double>,
                                   GtH5::Vector<int>>
                   >::value, "Bufer element type mismatch!");
    static_assert (std::is_same<
                        GtH5::buffer_t<GtH5::Comp<QPoint, QString>>,
                        GtH5::Comp<GtH5::Vector<QByteArray>,
                                   GtH5::Vector<QPoint>>
                   >::value, "Bufer type mismatch!");
    static_assert (std::is_same<
                        GtH5::buffer_t<GtH5::Comp<GtH5::VarLen<QPoint>, hsize_t>>,
                        GtH5::Comp<GtH5::Vector<hsize_t>,
                                   GtH5::Vector<GtH5::details::hvl_buffer<QPoint>>>
                   >::value, "Bufer type mismatch!");
}
