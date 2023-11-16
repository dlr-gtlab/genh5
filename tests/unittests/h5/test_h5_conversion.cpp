/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 23.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#define GENH5_USE_QT_BINDINGS
#include "genh5_conversion.h"

#include <QPoint>

struct TestConversion : public testing::Test
{
    template<typename Tsrc, typename Tdest = Tsrc>
    void assertConversionType() const
    {
        static_assert(std::is_same<GenH5::conversion_t<Tsrc>, Tdest>::value,
                      "Conversion type mismatch!");
    }
    template<typename Tsrc, typename Tdest = Tsrc>
    void assertBufferType() const
    {
        static_assert(std::is_same<GenH5::buffer_element_t<Tsrc>, Tdest>::value,
                      "Buffer type mismatch!");
    }

    template<typename Tsrc, typename Tdest = Tsrc>
    void assertCompConversionType() const
    {
        static_assert(std::is_same<GenH5::conversion_t<Tsrc>,
                                   GenH5::mpl::reversed_comp_t<Tdest>>::value,
                      "Conversion type mismatch!");
    }
    template<typename Tsrc, typename Tdest = Tsrc>
    void assertCompBufferType() const
    {
        static_assert(std::is_same<GenH5::buffer_element_t<Tsrc>,
                                   GenH5::mpl::reversed_comp_t<Tdest>>::value,
                      "Buffer type mismatch!");
    }
};

TEST_F(TestConversion, implicitConversionTypes)
{
    assertConversionType<bool>();

    assertConversionType<char>();
    assertConversionType<char*>();
    assertConversionType<char const*>();

    assertConversionType<int>();
    assertConversionType<long int>();
    assertConversionType<long long int>();
    assertConversionType<unsigned int>();
    assertConversionType<unsigned long int>();
    assertConversionType<unsigned long long int>();

    assertConversionType<float>();
    assertConversionType<double>();

    assertConversionType<GenH5::Version>();
}

TEST_F(TestConversion, implicitBufferTypes)
{
    assertBufferType<bool>();

    assertBufferType<char>();
    assertBufferType<char*>();
    assertBufferType<char const*>();

    assertBufferType<int>();
    assertBufferType<long int>();
    assertBufferType<long long int>();
    assertBufferType<unsigned int>();
    assertBufferType<unsigned long int>();
    assertBufferType<unsigned long long int>();

    assertBufferType<float>();
    assertBufferType<double>();

    assertBufferType<GenH5::Version>();
}

TEST_F(TestConversion, bindings_conversionTypes)
{
    /** STL **/
    assertConversionType<std::string, char*>();

    /** Qt **/
    assertConversionType<QString, char*>();
    assertConversionType<QByteArray, char*>();
}

TEST_F(TestConversion, bindings_bufferTypes)
{
    /** STL **/
    assertBufferType<std::string, GenH5::ByteArray>();

    /** Qt **/
    assertBufferType<QString, GenH5::ByteArray>();
    assertBufferType<QByteArray, GenH5::ByteArray>();
}

// QPOINT
struct PointData{ int xp, yp; };

GENH5_DECLARE_CONVERSION_TYPE(QPoint, PointData);

GENH5_DECLARE_CONVERSION(QPoint, value, /*buffer*/)
{
    return PointData{value.x(), value.y()};
}

GENH5_DECLARE_REVERSE_CONVERSION(QPoint, PointData, value)
{
    return QPoint{value.xp, value.yp};
}

TEST_F(TestConversion, custom_conversionTypes)
{
    assertConversionType<QPoint, PointData>();
}

TEST_F(TestConversion, custom_bufferTypes)
{
    assertBufferType<QPoint>();
}

TEST_F(TestConversion, array_conversionTypes)
{
    using GenH5::Array;

    assertConversionType<Array<int, 12>, Array<int, 12>>();
    assertConversionType<Array<double, 30>, Array<double, 30>>();
    assertConversionType<Array<QString, 42>, Array<char*, 42>>();

    assertConversionType<size_t[25], Array<size_t, 25>>();
    assertConversionType<char[25], Array<char, 25>>();

    // custom
    assertConversionType<Array<QPoint, 11>, Array<PointData, 11>>();
}

TEST_F(TestConversion, array_bufferTypes)
{
    using GenH5::Array;

    assertBufferType<Array<int, 12>, int>();
    assertBufferType<Array<double, 42>, double>();
    assertBufferType<Array<QString, 42>, GenH5::ByteArray>();

    assertBufferType<size_t[25], size_t>();
    assertBufferType<char[25], char>();

    // custom
    assertBufferType<Array<QPoint, 11>, QPoint>();
}

TEST_F(TestConversion, varlen_conversionTypes)
{
    using GenH5::VarLen;

    assertConversionType<VarLen<int>, hvl_t>();
    assertConversionType<VarLen<double>, hvl_t>();
    assertConversionType<VarLen<QString>, hvl_t>();
    assertConversionType<VarLen<char*>, hvl_t>();

    // custom
    assertConversionType<VarLen<QPoint>, hvl_t>();
}

TEST_F(TestConversion, varlen_bufferTypes)
{
    using GenH5::VarLen;

    assertBufferType<VarLen<int>, GenH5::details::hvl_buffer<int>>();
    assertBufferType<VarLen<double>, GenH5::details::hvl_buffer<double>>();
    assertBufferType<VarLen<QString>, GenH5::details::hvl_buffer<QString>>();
    assertBufferType<VarLen<char*>, GenH5::details::hvl_buffer<char*>>();

    // custom
    assertBufferType<VarLen<QPoint>, GenH5::details::hvl_buffer<QPoint>>();
}

TEST_F(TestConversion, compound_conversionTypes)
{
    using GenH5::Comp;

    assertCompConversionType<Comp<int, double>>();
    assertCompConversionType<Comp<double>>();
    assertCompConversionType<Comp<QString, QPoint>, Comp<char*, PointData>>();
    assertCompConversionType<Comp<int, double, size_t, QPoint>,
                             Comp<int, double, size_t, PointData>>();
}

TEST_F(TestConversion, compound_bufferTypes)
{
    using GenH5::Comp;
    using GenH5::Vector;

    assertCompBufferType<Comp<int, double>,
                         Comp<Vector<int>, Vector<double>>>();
    assertCompBufferType<Comp<double>,
                         Comp<Vector<double>>>();
    assertCompBufferType<Comp<QString, QPoint>,
                         Comp<Vector<GenH5::ByteArray>, Vector<QPoint>>>();
    assertCompBufferType<Comp<int, double, size_t, QPoint>,
                         Comp<Vector<int>, Vector<double>,
                              Vector<size_t>, Vector<QPoint>>>();
}

TEST_F(TestConversion, complex_conversionTypes)
{
    using GenH5::Comp;
    using GenH5::RComp;
    using GenH5::Vector;
    using GenH5::VarLen;
    using GenH5::Array;

    assertCompConversionType<Comp<int, VarLen<QString>, Array<double, 42>>,
                             Comp<int, hvl_t, Array<double, 42>>>();
    assertCompConversionType<Comp<Array<size_t, 1>, Comp<VarLen<int>, QString>>,
                             Comp<Array<size_t, 1>, RComp<hvl_t, char*>>>();

    assertConversionType<VarLen<Array<double, 42>>,
                         hvl_t>();
    assertConversionType<VarLen<Comp<double, QPoint>>,
                         hvl_t>();

    assertConversionType<Array<Comp<QString, GenH5::Version>, 42>,
                         Array<RComp<char*, GenH5::Version>, 42>>();
    assertConversionType<VarLen<int>[5], Array<hvl_t, 5>>();
    assertConversionType<Array<QPoint, 15>[5],
                         Array<Array<PointData, 15>, 5>>();
}

TEST_F(TestConversion, complex_bufferTypes)
{
    using GenH5::Comp;
    using GenH5::RComp;
    using GenH5::Vector;
    using GenH5::VarLen;
    using GenH5::Array;

    assertCompBufferType<Comp<int, VarLen<QString>, Array<double, 42>>,
                         Comp<Vector<int>,
                              Vector<GenH5::details::hvl_buffer<QString>>,
                              Vector<double>>
                         >();
    assertCompBufferType<Comp<Array<size_t, 1>, Comp<VarLen<int>, QString>>,
                         Comp<Vector<size_t>,
                              RComp<Vector<GenH5::details::hvl_buffer<int>>,
                                    Vector<GenH5::ByteArray>>
                              >
                          >();

    assertBufferType<VarLen<Array<double, 42>>,
                     GenH5::details::hvl_buffer<Array<double, 42>>>();
    assertBufferType<VarLen<Comp<double, QPoint>>,
                     GenH5::details::hvl_buffer<Comp<double, QPoint>>>();

    assertBufferType<Array<Comp<QString, GenH5::Version>, 42>,
                     RComp<Vector<GenH5::ByteArray>, Vector<GenH5::Version>>>();

    assertBufferType<VarLen<int>[5],
                     GenH5::details::hvl_buffer<int>>();
    assertBufferType<Array<QPoint, 15>[5],
                     QPoint>();
}

TEST_F(TestConversion, convertPod)
{
    using GenH5::convert;

    EXPECT_EQ(convert(true), true);

    EXPECT_EQ(convert('X'), 'X');
    char c1[] = "Test";
    EXPECT_STREQ(convert(c1), c1);
    const char* c2 = "Hello World";
    EXPECT_STREQ(convert(c2), c2);

    EXPECT_EQ(convert(-14), -14);
    EXPECT_EQ(convert(-42l), -42l);
    EXPECT_EQ(convert(-78293ll), -78293ll);

    EXPECT_EQ(convert(14u), 14u);
    EXPECT_EQ(convert(42ul), 42ul);
    EXPECT_EQ(convert(78293ull), 78293ull);

    EXPECT_FLOAT_EQ(convert(42.1f), 42.1f);
    EXPECT_DOUBLE_EQ(convert(42.123456), 42.123456);

    EXPECT_TRUE(convert(GenH5::Version{1, 2, 3}) == (GenH5::Version{1, 2, 3}));
}

TEST_F(TestConversion, convertStrings)
{
    using GenH5::convert;
    using GenH5::convertTo;

    // buffer for strings
    GenH5::Vector<GenH5::ByteArray> buffer;
    buffer.reserve(3);

    /** QT **/
    char const* c1_ = "hello world";
    char* c1 = convert(QByteArray{"hello world"}, buffer);
    EXPECT_EQ(buffer.size(), 1);
    EXPECT_STREQ(c1, c1_);

    EXPECT_EQ(QByteArray{c1}, convertTo<QByteArray>(c1));

    char const* c2_ = "Test";
    char* c2 = convert(QString{"Test"}, buffer);
    EXPECT_EQ(buffer.size(), 2);
    EXPECT_STREQ(c2, c2_);

    EXPECT_EQ(QString{c2}, convertTo<QString>(c2));

    /** STL **/
    char const* c3_ = "Fancy String";
    char* c3 = convert(std::string{"Fancy String"}, buffer);
    EXPECT_EQ(buffer.size(), 3);
    EXPECT_STREQ(c3, c3_);

    EXPECT_EQ(std::string{c3}, convertTo<std::string>(c3));

    // cross conversions (they shares the same conversion_t)
    EXPECT_EQ(std::string{c1}, convertTo<std::string>(c1));
    EXPECT_EQ(QByteArray{c2}, convertTo<QByteArray>(c2));
    EXPECT_EQ(QString{c3}, convertTo<QString>(c3));
}

TEST_F(TestConversion, convertCustom)
{
    using GenH5::convert;
    using GenH5::convertTo;

    QPoint pointOrig{1, 2};
    GenH5::buffer_t<QPoint> buffer;
    PointData res = convert(pointOrig, buffer);

    QPoint point = convertTo<QPoint>(res);
    EXPECT_EQ(point, pointOrig);
}

TEST_F(TestConversion, convertArray)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::Array;

    using ArrayT = Array<QByteArray, 4>;
    ArrayT arrayOrig{"Hello World", "ABC", "Fancy String", "DEF"};

    GenH5::buffer_t<ArrayT> buffer;
    buffer.reserve(4);
    Array<char*, 4> res = convert(arrayOrig, buffer);

    EXPECT_EQ(res.size(), arrayOrig.size());
    EXPECT_EQ(buffer.size(), arrayOrig.size());

    ArrayT array = convertTo<ArrayT>(res);
    EXPECT_EQ(array, arrayOrig);
}

TEST_F(TestConversion, convertCArray)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::Array;

    using ArrayT = std::string[5];
    ArrayT arrayOrig{"Hello World", "ABC", "Fancy String", "DEF"};

    GenH5::buffer_t<ArrayT> buffer;
    buffer.reserve(5);
    Array<char*, 5> res = convert(arrayOrig, buffer);

    EXPECT_EQ(res.size(), 5);
    EXPECT_EQ(buffer.size(), 5);

    Array<std::string, 5> array = convertTo<ArrayT>(res);
    EXPECT_TRUE(std::equal(std::cbegin(array), std::cend(array),
                           std::cbegin(arrayOrig)));
}

TEST_F(TestConversion, convertCString)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::Array;

    using ArrayT = char[5];
    ArrayT arrayOrig = {"Test"};
    GenH5::buffer_t<ArrayT> buffer;

    Array<char, 5> res = convert(arrayOrig, buffer);

    EXPECT_EQ(res.size(), 5);
    EXPECT_EQ(buffer.size(), 0);

    Array<char, 5> array = convertTo<ArrayT>(res);
    EXPECT_TRUE(std::equal(std::cbegin(array), std::cend(array),
                           std::cbegin(arrayOrig)));
}

TEST_F(TestConversion, convertArrayComp)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::Array;
    using GenH5::Comp;

    using CompT  = Comp<QByteArray, double>;
    using ArrayT = Array<CompT, 4>;

    CompT arrayOrig[4] { // first create c-array
        CompT{"QPoint{1,1}", 2},
        CompT{"QPoint{1,2}", 42.1},
        CompT{"QPoint{2,0}", 0.12},
        CompT{"{}", .1}
    };

    GenH5::buffer_t<CompT[4]> buffer;
    std::get<1>(buffer).reserve(4);
    Array<Comp<double, char*>, 4> res = convert(arrayOrig, buffer);

    ArrayT array = convertTo<ArrayT>(res);
    EXPECT_TRUE(std::equal(std::cbegin(array), std::cend(array),
                           std::cbegin(arrayOrig)));
}

TEST_F(TestConversion, convertVarLen)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::VarLen;
    using VarLenT = VarLen<QString>;

    VarLenT varlenOrig{"Hello World", "ABC", "Fancy String", "DEF"};

    GenH5::buffer_t<VarLen<QString>> buffer;
    hvl_t res = convert(varlenOrig, buffer);

    ASSERT_EQ(buffer.size(), 1);
    auto& first = buffer.front();
    EXPECT_EQ(res.p, first.data.data());
    EXPECT_EQ(res.len, first.data.size());
    EXPECT_EQ(res.len, varlenOrig.size());
    EXPECT_EQ(first.buffer.size(), varlenOrig.size());

    VarLenT varlen = convertTo<VarLenT>(res);
    EXPECT_EQ(varlen, varlenOrig);
}

TEST_F(TestConversion, convertVarLenCustom)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::VarLen;
    using VarLenT = VarLen<QPoint>;

    VarLenT varlenOrig{{0, 1}, {1, 1}, {1, 2}};

    GenH5::buffer_t<VarLenT> buffer;
    hvl_t res = convert(varlenOrig, buffer);

    static_assert (std::is_same<hvl_t, decltype (res)>::value,
                   "Conversion type mismatch");

    ASSERT_EQ(buffer.size(), 1);
    auto& first = buffer.front();
    EXPECT_EQ(res.p, first.data.data());
    EXPECT_EQ(res.len, first.data.size());
    EXPECT_EQ(res.len, varlenOrig.size());

    EXPECT_EQ(first.buffer.size(), 0);

    VarLenT varlen = convertTo<VarLenT>(res);
    EXPECT_EQ(varlen, varlenOrig);
}

TEST_F(TestConversion, convertCompound)
{
    using GenH5::convert;
    using GenH5::convertTo;
    using GenH5::Comp;
    using GenH5::get;
    using Tuple = Comp<QString, size_t, double>;

    Tuple tupleOrig{"Hello World", 42, 0.12};

    GenH5::buffer_t<Tuple> buffer;
    Comp<double, size_t, char*> res = convert(tupleOrig, buffer);

    Tuple tuple = convertTo<Tuple>(res);
    EXPECT_EQ(get<0>(tuple), QString{"Hello World"});
    EXPECT_EQ(get<1>(tuple), 42);
    EXPECT_EQ(get<2>(tuple), 0.12);
}

