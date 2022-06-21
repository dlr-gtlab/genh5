/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "gth5_data.h"

#include "testhelper.h"

#include <QPoint>

/// This is a test fixture that does a init for each test
class TestH5Data : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        intData    = {1, 2, 3, 4, 5};
        doubleData = {1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5"};
    }

    GtH5::Vector<int> intData;
    GtH5::Vector<double> doubleData;
    QStringList stringData;

    template<typename T>
    bool testSerialize(GtH5::Vector<T> const& values)
    {
        GtH5::Data<T> data{values};
        EXPECT_EQ(data.c(), values);
        bool success = (data.c() == values);

        EXPECT_EQ(data.size(), values.size());
        success &= (values.size() == data.size());

        auto desValues = data.deserialize();
        EXPECT_EQ(desValues, values);
        success &= (desValues == values);

        return success;
    }
};

GTH5_DECLARE_IMPLICIT_CONVERSION(QPoint);

GTH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    return GtH5::dataType<T, T>({"xp", "yp"});
};

TEST_F(TestH5Data, serializationPod)
{
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<int>(10)));
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<long>(10)));
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<float>(10)));
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<double>(10)));
    EXPECT_TRUE(testSerialize(GtH5::Vector<char const*>{
        "Abc", "Def", "Geh", "char const*#!^"
    }));
    EXPECT_TRUE(testSerialize(GtH5::Vector<char>{'C', '\n', '#', '\0'}));

//    EXPECT_TRUE((testSerialize(GtH5::vector_t<bool>{true, false})));
}

TEST_F(TestH5Data, serializationStrings)
{
    { // QByteArray
        auto values = h5TestHelper->randomByteArrays(5);

        GtH5::Data<QByteArray> data{values};
        EXPECT_EQ(data.size(), values.size());

        auto desValues = data.deserialize();
        EXPECT_EQ(desValues, values);

        auto value0 = data.deserializeIdx(0);
        EXPECT_EQ(value0, values[0]);
    }

    { // QString
        auto values = h5TestHelper->randomStringList(5);

        GtH5::Data<QString> data{values};
        EXPECT_EQ(data.size(), values.size());

        auto desValues = data.deserialize<QStringList>();
        EXPECT_EQ(desValues, values);

        auto value0 = data.deserializeIdx(0);
        EXPECT_EQ(value0, values[0]);
    }
}

TEST_F(TestH5Data, serializationCompound)
{
    auto comData = GtH5::makeCompData(stringData, intData, doubleData);
    // data must be deserialized first
    GtH5::Vector<QString> desStrData;
    GtH5::Vector<int> desIntData;
    GtH5::Vector<double> desDoubleData;

    comData.deserialize(desStrData, desIntData, desDoubleData);

    EXPECT_EQ(desStrData.toList(), stringData);
    EXPECT_EQ(desIntData, intData);
    EXPECT_EQ(desDoubleData, doubleData);

    auto tupleAt0 = comData.deserializeIdx(0);
    QString s;
    int i;
    double d;
    GtH5::unpack(tupleAt0, s, i, d);
    EXPECT_EQ(s, stringData[0]);
    EXPECT_EQ(i, intData[0]);
    EXPECT_EQ(d, doubleData[0]);
}

TEST_F(TestH5Data, serializationComplex)
{
    GtH5::CompData<GtH5::VarLen<double>, QString> data{
        GtH5::Comp<GtH5::VarLen<double>, QString>{
            doubleData,
            "Fancy String"
        }
    };

    GtH5::Vector<GtH5::VarLen<double>> doubles;
    GtH5::Vector<QString> strings;
    data.deserialize(doubles, strings);

    ASSERT_EQ(doubles.size(), 1);
    ASSERT_EQ(strings.size(), 1);

    EXPECT_EQ(doubles.at(0), doubleData);
    EXPECT_EQ(strings.at(0), QString{"Fancy String"});
}

TEST_F(TestH5Data, array)
{
    using ArrayT = GtH5::Array<QPoint, 6>;
    ArrayT points = {
        QPoint{11, 1},
        QPoint{12, 2},
        QPoint{-4, -2},
        QPoint{5, 60}
    };
    GtH5::Data<ArrayT> data{points};

    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.dataSpace().size(), 1);
    EXPECT_TRUE(data.dataType().isValid());
    EXPECT_TRUE(data.dataType().isArray());
    EXPECT_TRUE(data.dataType().arrayDimensions() ==
                GtH5::Dimensions{points.size()});

    ArrayT desData = data.deserializeIdx(0);

    EXPECT_EQ(desData.size(), points.size());
    EXPECT_EQ(desData, points);
}

TEST_F(TestH5Data, varlen)
{
    auto doubleData = h5TestHelper->linearDataVector<float>(42, 1, 1.1f);
    GtH5::Data<GtH5::VarLen<float>> data{doubleData};

    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.dataSpace().size(), 1);
    EXPECT_TRUE(data.dataType().isValid());
    EXPECT_TRUE(data.dataType().isVarLen());
    EXPECT_TRUE(data.dataType().superType() == GtH5::dataType<float>());

    GtH5::VarLen<float> desData = data.deserializeIdx(0);

    EXPECT_EQ(desData.size(), doubleData.size());
    EXPECT_EQ(desData, doubleData);
}

TEST_F(TestH5Data, compound)
{
    QStringList sList{"Hello World", "Fany String"};
    GtH5::Vector<double> dList{42.2, 420};

    GtH5::CompData<QString, double> data{
        GtH5::Vector<GtH5::Comp<QString, double>>{
            {sList.front(), dList.front()},
            {sList.back(), dList.back()}
        }
    };

    ASSERT_EQ(data.size(), 2);
    EXPECT_EQ(data.dataSpace().size(), 2);
    EXPECT_TRUE((data.dataType() == GtH5::dataType<QString, double>()));

    GtH5::Vector<QString> desStrData;
    GtH5::Vector<double> desDoubleData;
    data.deserialize(desStrData, desDoubleData);

    EXPECT_EQ(desStrData.toList(), sList);
    EXPECT_EQ(desDoubleData, dList);
}

TEST_F(TestH5Data, reserveBuffer)
{
    using GtH5::details::reserveBuffer;
    size_t bufferSize = 10;

    // reserving should not be necessary
    GtH5::buffer_t<double> buffer1;
    reserveBuffer<double>(buffer1, bufferSize);
    EXPECT_EQ(buffer1.capacity(), 0);

    GtH5::buffer_t<QPoint> buffer2;
    reserveBuffer<QPoint>(buffer2, bufferSize);
    EXPECT_EQ(buffer2.capacity(), 0);

    // reserving is necessary
    GtH5::buffer_t<QString> buffer3;
    reserveBuffer<QString>(buffer3, bufferSize);
    EXPECT_EQ(buffer3.capacity(), bufferSize);

    GtH5::buffer_t<QByteArray> buffer4;
    reserveBuffer<QByteArray>(buffer4, bufferSize);
    EXPECT_EQ(buffer4.capacity(), bufferSize);

    using CompT = GtH5::Comp<QString, int, QPoint>;
    GtH5::buffer_t<CompT> buffer5;
    reserveBuffer<CompT>(buffer5, bufferSize);
    EXPECT_EQ(std::get<2>(buffer5).capacity(), 10); // = QString
    EXPECT_EQ(std::get<1>(buffer5).capacity(), 0); // = int
    EXPECT_EQ(std::get<0>(buffer5).capacity(), 0); // = QPoint
}

TEST_F(TestH5Data, dataCopy)
{
    auto strData = GtH5::makeData(stringData);

    auto data = strData;

    // data must be deserialized first
    auto desStrData = data.deserialize();

    EXPECT_EQ(desStrData.toList(), stringData);
}

TEST_F(TestH5Data, dataAssign)
{
    GtH5::Vector<QPoint> points{{11, 1}, {42, 12}};
    auto pData = GtH5::makeData(points);

    GtH5::Data<QPoint> data;
    data = pData;

    // data must be deserialized first
    auto desPData = data.deserialize();

    EXPECT_EQ(desPData, points);
}

TEST_F(TestH5Data, dataCompoundCopy)
{
    auto comData = GtH5::makeCompData(intData, doubleData, stringData);

    auto data{comData};

    // data must be deserialized first
    QStringList desStrData;
    GtH5::Vector<int> desIntData;
    GtH5::Vector<double> desDoubleData;

    data.deserialize(desIntData, desDoubleData, desStrData);

    EXPECT_EQ(desIntData, intData);
    EXPECT_EQ(desDoubleData, doubleData);
    EXPECT_EQ(desStrData, stringData);
}

TEST_F(TestH5Data, dataCompoundAssign)
{
    auto comData = GtH5::makeCompData(doubleData, intData, stringData);

    GtH5::CompData<double, int, QString> data;
    data = comData;

    // data must be deserialized first
    QStringList desStrData;
    GtH5::Vector<int> desIntData;
    GtH5::Vector<double> desDoubleData;

    data.deserialize(desDoubleData, desIntData, desStrData);

    EXPECT_EQ(desIntData, intData);
    EXPECT_EQ(desDoubleData, doubleData);
    EXPECT_EQ(desStrData, stringData);
}
