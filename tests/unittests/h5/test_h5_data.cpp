/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_data.h"

#include "testhelper.h"

#include <QPoint>
#include <QStringList>

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

    GenH5::Vector<int> intData;
    GenH5::Vector<double> doubleData;
    QStringList stringData;

    template<typename T>
    bool testSerialize(GenH5::Vector<T> const& values)
    {
        GenH5::Data<T> data{values};
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

GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);

GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    return GenH5::dataType<T, T>({"xp", "yp"});
};

TEST_F(TestH5Data, serializationPod)
{
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<int>(10)));
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<long>(10)));
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<float>(10)));
    EXPECT_TRUE(testSerialize(h5TestHelper->randomDataVector<double>(10)));
    EXPECT_TRUE(testSerialize(GenH5::Vector<char const*>{
        "Abc", "Def", "Geh", "char const*#!^"
    }));
    EXPECT_TRUE(testSerialize(GenH5::Vector<char>{'C', '\n', '#', '\0'}));

//    EXPECT_TRUE((testSerialize(GenH5::vector_t<bool>{true, false})));
}

TEST_F(TestH5Data, serializationStrings)
{
    { // QByteArray
        auto values = h5TestHelper->randomByteArrays(5);

        GenH5::Data<QByteArray> data{values};
        EXPECT_EQ(data.size(), values.size());

        auto desValues = data.deserialize();
        EXPECT_EQ(desValues, values);

        auto value0 = data.deserializeIdx(0);
        EXPECT_EQ(value0, values[0]);
    }

    { // QString
        auto values = h5TestHelper->randomStringList(5);

        GenH5::Data<QString> data{values};
        EXPECT_EQ(data.size(), values.size());

        auto desValues = data.deserialize<QStringList>();
        EXPECT_EQ(desValues, values);

        auto value0 = data.deserializeIdx(0);
        EXPECT_EQ(value0, values[0]);
    }
}

TEST_F(TestH5Data, serializationCompound)
{
    auto comData = GenH5::makeCompData(stringData, intData, doubleData);
    // data must be deserialized first
    GenH5::Vector<QString> desStrData;
    GenH5::Vector<int> desIntData;
    GenH5::Vector<double> desDoubleData;

    comData.deserialize(desStrData, desIntData, desDoubleData);

    EXPECT_EQ(desStrData.toList(), stringData);
    EXPECT_EQ(desIntData, intData);
    EXPECT_EQ(desDoubleData, doubleData);

    auto tupleAt0 = comData.deserializeIdx(0);
    QString s;
    int i;
    double d;
    GenH5::unpack(tupleAt0, s, i, d);
    EXPECT_EQ(s, stringData[0]);
    EXPECT_EQ(i, intData[0]);
    EXPECT_EQ(d, doubleData[0]);
}

TEST_F(TestH5Data, serializationComplex)
{
    GenH5::CompData<GenH5::VarLen<double>, QString> data{
        GenH5::Comp<GenH5::VarLen<double>, QString>{
            doubleData,
            "Fancy String"
        }
    };

    GenH5::Vector<GenH5::VarLen<double>> doubles;
    GenH5::Vector<QString> strings;
    data.deserialize(doubles, strings);

    ASSERT_EQ(doubles.size(), 1);
    ASSERT_EQ(strings.size(), 1);

    EXPECT_EQ(doubles.at(0), doubleData);
    EXPECT_EQ(strings.at(0), QString{"Fancy String"});
}

TEST_F(TestH5Data, array)
{
    using ArrayT = GenH5::Array<QPoint, 6>;
    ArrayT points = {
        QPoint{11, 1},
        QPoint{12, 2},
        QPoint{-4, -2},
        QPoint{5, 60}
    };
    GenH5::Data<ArrayT> data{points};

    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.dataSpace().size(), 1);
    EXPECT_TRUE(data.dataType().isValid());
    EXPECT_TRUE(data.dataType().isArray());
    EXPECT_TRUE(data.dataType().arrayDimensions() ==
                GenH5::Dimensions{points.size()});

    ArrayT desData = data.deserializeIdx(0);

    EXPECT_EQ(desData.size(), points.size());
    EXPECT_EQ(desData, points);
}

TEST_F(TestH5Data, varlen)
{
    auto doubleData = h5TestHelper->linearDataVector<float>(42, 1, 1.1f);
    GenH5::Data<GenH5::VarLen<float>> data{doubleData};

    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.dataSpace().size(), 1);
    EXPECT_TRUE(data.dataType().isValid());
    EXPECT_TRUE(data.dataType().isVarLen());
    EXPECT_TRUE(data.dataType().superType() == GenH5::dataType<float>());

    GenH5::VarLen<float> desData = data.deserializeIdx(0);

    EXPECT_EQ(desData.size(), doubleData.size());
    EXPECT_EQ(desData, doubleData);
}

TEST_F(TestH5Data, compound)
{
    QStringList sList{"Hello World", "Fany String"};
    GenH5::Vector<double> dList{42.2, 420};

    GenH5::CompData<QString, double> data{
        GenH5::Vector<GenH5::Comp<QString, double>>{
            {sList.front(), dList.front()},
            {sList.back(), dList.back()}
        }
    };

    ASSERT_EQ(data.size(), 2);
    EXPECT_EQ(data.dataSpace().size(), 2);
    EXPECT_TRUE((data.dataType() == GenH5::dataType<QString, double>()));

    GenH5::Vector<QString> desStrData;
    GenH5::Vector<double> desDoubleData;
    data.deserialize(desStrData, desDoubleData);

    EXPECT_EQ(desStrData.toList(), sList);
    EXPECT_EQ(desDoubleData, dList);
}

TEST_F(TestH5Data, reserveBuffer)
{
    using GenH5::details::reserveBuffer;
    size_t bufferSize = 10;

    // reserving should not be necessary
    GenH5::buffer_t<double> buffer1;
    reserveBuffer<double>(buffer1, bufferSize);
    EXPECT_EQ(buffer1.capacity(), 0);

    GenH5::buffer_t<QPoint> buffer2;
    reserveBuffer<QPoint>(buffer2, bufferSize);
    EXPECT_EQ(buffer2.capacity(), 0);

    // reserving is necessary
    GenH5::buffer_t<QString> buffer3;
    reserveBuffer<QString>(buffer3, bufferSize);
    EXPECT_EQ(buffer3.capacity(), bufferSize);

    GenH5::buffer_t<QByteArray> buffer4;
    reserveBuffer<QByteArray>(buffer4, bufferSize);
    EXPECT_EQ(buffer4.capacity(), bufferSize);

    using CompT = GenH5::Comp<QString, int, QPoint>;
    GenH5::buffer_t<CompT> buffer5;
    reserveBuffer<CompT>(buffer5, bufferSize);
    EXPECT_EQ(std::get<2>(buffer5).capacity(), 10); // = QString
    EXPECT_EQ(std::get<1>(buffer5).capacity(), 0); // = int
    EXPECT_EQ(std::get<0>(buffer5).capacity(), 0); // = QPoint
}

TEST_F(TestH5Data, dataCopy)
{
    auto strData = GenH5::makeData(stringData);

    auto data = strData;

    // data must be deserialized first
    auto desStrData = data.deserialize();

    EXPECT_EQ(desStrData.toList(), stringData);
}

TEST_F(TestH5Data, dataAssign)
{
    GenH5::Vector<QPoint> points{{11, 1}, {42, 12}};
    auto pData = GenH5::makeData(points);

    GenH5::Data<QPoint> data;
    data = pData;

    // data must be deserialized first
    auto desPData = data.deserialize();

    EXPECT_EQ(desPData, points);
}

TEST_F(TestH5Data, dataCompoundCopy)
{
    auto comData = GenH5::makeCompData(intData, doubleData, stringData);

    auto data{comData};

    // data must be deserialized first
    QStringList desStrData;
    GenH5::Vector<int> desIntData;
    GenH5::Vector<double> desDoubleData;

    data.deserialize(desIntData, desDoubleData, desStrData);

    EXPECT_EQ(desIntData, intData);
    EXPECT_EQ(desDoubleData, doubleData);
    EXPECT_EQ(desStrData, stringData);
}

TEST_F(TestH5Data, dataCompoundAssign)
{
    auto comData = GenH5::makeCompData(doubleData, intData, stringData);

    GenH5::CompData<double, int, QString> data;
    data = comData;

    // data must be deserialized first
    QStringList desStrData;
    GenH5::Vector<int> desIntData;
    GenH5::Vector<double> desDoubleData;

    data.deserialize(desDoubleData, desIntData, desStrData);

    EXPECT_EQ(desIntData, intData);
    EXPECT_EQ(desDoubleData, doubleData);
    EXPECT_EQ(desStrData, stringData);
}
