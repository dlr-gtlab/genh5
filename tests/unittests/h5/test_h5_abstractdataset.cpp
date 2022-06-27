/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_dataset.h"
#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_data.h"
#include "genh5_attribute.h"

#include "testhelper.h"

#include <QDebug>
#include <QPoint>
#include <QPointF>

/// This is a test fixture that does a init for each test
class TestH5AbstractDataSet : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        emptyData  = QVector<int>();
        intData    = QVector<int>{1, 2, 3, 4, 5, 6};
        doubleData = QVector<double>{1, 2, 3, 4, 5, 6};

        file = GenH5::File(h5TestHelper->newFilePath(),
                           GenH5::Create);
        ASSERT_TRUE(file.isValid());
        root = file.root();
        ASSERT_TRUE(root.isValid());

        dataset = root.createDataset(QByteArrayLiteral("test"),
                                     intData.dataType(),
                                     intData.dataSpace());
        ASSERT_TRUE(dataset.isValid());
    }

    GenH5::File file;
    GenH5::Group root;
    GenH5::DataSet dataset;

    GenH5::Data<int> emptyData;
    GenH5::Data<int> intData;
    GenH5::Data<double> doubleData;

    /*
    template<typename T>
    bool testSimpleRW(GenH5::vector_t<T> const& values, int& counter)
    {
        // for easier debugging
        qDebug() << __func__ << counter;

        GenH5::Data<T> data{values};

        // create new dataset
        auto dset = root.createDataset(QByteArrayLiteral("test_") +
                                       QByteArray::number(counter++),
                                       data.dataType(),
                                       data.dataSpace());
        EXPECT_TRUE(dset.isValid());
        EXPECT_TRUE(dset.dataSpace() == data.dataSpace());
        EXPECT_TRUE(dset.dataType() == data.dataType());
        bool success = dset.isValid() && dset.dataSpace() == data.dataSpace() &&
                dset.dataType() == data.dataType();

        // write data
        success &= dset.write(data);
        EXPECT_TRUE(success);

        // read data
        GenH5::Data<T> readData;
        success &= dset.read(readData);
        EXPECT_TRUE(success);
        success &= (readData.data() == values);
        EXPECT_EQ(readData.data(), values);

        // test deserialize
        GenH5::vector_t<T> desValues;
        readData.deserialize(desValues);
        success &= (desValues == values);
        EXPECT_EQ(desValues, values);

        return success;
    }
    */

    template<typename T, typename TContainer>
    bool testRW(TContainer const& values, int& counter)
    {
        // for easier debugging
        qDebug() << __func__ << counter;

        GenH5::Data<T> data{values};

        // create new dataset
        auto dset = root.createDataset(QByteArrayLiteral("test_") +
                                       QByteArray::number(counter++),
                                       data.dataType(),
                                       data.dataSpace());
        EXPECT_TRUE(dset.isValid());
        EXPECT_TRUE(dset.dataSpace() == data.dataSpace());
        EXPECT_TRUE(dset.dataType() == data.dataType());
        bool success = dset.isValid() && dset.dataSpace() == data.dataSpace() &&
                dset.dataType() == data.dataType();

        // write data
        success &= dset.write(data);
        EXPECT_TRUE(success);

        // read data
        GenH5::Data<T> readData;
        success &= dset.read(readData);
        EXPECT_TRUE(success);

        // test deserialize
        auto desValues = readData.template deserialize<TContainer>();
        success &= (desValues == values);
        EXPECT_EQ(desValues, values);

        return success;
    }
};

GENH5_DECLARE_IMPLICIT_CONVERSION(QPointF);
GENH5_DECLARE_DATATYPE_IMPL(QPointF)
{
    using Tp = decltype (std::declval<QPointF>().x());
    return GenH5::dataType<Tp, Tp>({"xp", "yp"});
};

TEST_F(TestH5AbstractDataSet, typeInvalid)
{
    GenH5::DataSet dset;
    EXPECT_THROW(dset.dataType(), GenH5::DataTypeException);
    GenH5::Attribute attr;
    EXPECT_THROW(attr.dataType(), GenH5::DataTypeException);
}

TEST_F(TestH5AbstractDataSet, spaceInvalid)
{
    GenH5::DataSet dset;
    EXPECT_THROW(dset.dataSpace(), GenH5::DataSpaceException);
    GenH5::Attribute attr;
    EXPECT_THROW(attr.dataSpace(), GenH5::DataSpaceException);
}

TEST_F(TestH5AbstractDataSet, RWscalarVarString)
{
    auto dset = root.createDataset(QByteArray{"my_dat"},
                                   GenH5::DataType::VarString,
                                   GenH5::DataSpace::Scalar);

    ASSERT_TRUE(dset.isValid());

    QByteArray buffer{"my_data"};
    char* d = buffer.data();
    GenH5::Data<QString> data{d};
    EXPECT_TRUE(dset.write(data));

    data.clear();
    EXPECT_TRUE(dset.read(data));
    EXPECT_STREQ(data.at(0), d);
}

TEST_F(TestH5AbstractDataSet, RWscalarDouble)
{
    auto dset = root.createAttribute(QByteArray{"my_dat"},
                                     GenH5::DataType::Double,
                                     GenH5::DataSpace::Scalar);

    ASSERT_TRUE(dset.isValid());

    GenH5::Data<double> data{42.0};
    EXPECT_TRUE(dset.write(data));

    data.clear();
    EXPECT_TRUE(dset.read(data));
    EXPECT_EQ(data.at(0), 42);
}

TEST_F(TestH5AbstractDataSet, RWnull)
{
    auto dset = root.createDataset(QByteArray{"my_dat"},
                                   GenH5::DataType::Double,
                                   GenH5::DataSpace::Null);

    ASSERT_TRUE(dset.isValid());

    GenH5::Data<double> data{42.0};
    EXPECT_FALSE(dset.write(data));

    data.clear();
    EXPECT_FALSE(dset.read(data));
}

TEST_F(TestH5AbstractDataSet, validateDtypeAndDspace)
{
    EXPECT_TRUE(dataset.dataSpace() == intData.dataSpace());
    EXPECT_TRUE(dataset.dataType()  == intData.dataType());

    EXPECT_FALSE(dataset.dataSpace() == emptyData.dataSpace());
    EXPECT_TRUE(dataset.dataType()   == emptyData.dataType());

    EXPECT_TRUE(dataset.dataSpace() == doubleData.dataSpace());
    EXPECT_FALSE(dataset.dataType() == doubleData.dataType());
}

TEST_F(TestH5AbstractDataSet, write)
{
    // by void*
    EXPECT_TRUE(dataset.write(intData.data()));
    // false because of nullptr
    EXPECT_FALSE(dataset.write(nullptr));

    // using vector
    EXPECT_TRUE(dataset.write(intData.data()));
    // does not succed because size differs
    EXPECT_FALSE(dataset.write(emptyData.c()));
    // does succed because it does not check for dtype and sizes are equal
    EXPECT_TRUE(dataset.write(doubleData.data()));

    // using data vector
    EXPECT_TRUE(dataset.write(intData));
    // does not succed because size differs
    EXPECT_FALSE(dataset.write(emptyData));
    // does not succed because of dtype mismatch
//    EXPECT_FALSE(dataset.write(doubleData));
}

TEST_F(TestH5AbstractDataSet, read)
{
    // by void*
    EXPECT_TRUE(dataset.read(intData.data()));
    // nullptr not allowed
    EXPECT_FALSE(dataset.read(nullptr));

    // using vector
    EXPECT_TRUE(dataset.read(intData.data()));
    // does succed because vector will be allocated to the correct size
    EXPECT_TRUE(dataset.read(emptyData.c()));
    // does succed because vector will be allocated to the correct size and
    // does not check for dtype mismatch
    EXPECT_TRUE(dataset.read(doubleData.data()));

    // using vector
    EXPECT_TRUE(dataset.read(intData));
    // does succed because vector will be allocated to the correct size
    EXPECT_TRUE(dataset.read(emptyData));
    // does not succed because of dtype mismatch
//    EXPECT_FALSE(dataset.read(doubleData));
}

TEST_F(TestH5AbstractDataSet, RWtooFewElements)
{
    // when data.length != dataspace.sum
    // to few elements
    QVector<int> dataShort{intData.mid(0, 3)};
    // writing less than needed is not allowed
    EXPECT_FALSE(dataset.write(dataShort));
}

TEST_F(TestH5AbstractDataSet, RWtooManyElements)
{
    // when data.length != dataspace.sum
    // to many elements
    GenH5::Vector<int> dataLong = intData.mid(0, 3) + intData;

    // writing more than needed is technically allowed
    EXPECT_TRUE(dataset.write(dataLong));
    // but only the first entries were actually written
    GenH5::Data<int> readData;
    EXPECT_TRUE(dataset.read(readData));
    EXPECT_EQ(readData.c(), dataLong.mid(0, intData.length()));
}

TEST_F(TestH5AbstractDataSet, RWsimple)
{
    int idx = 0;
    EXPECT_TRUE(testRW<int>(h5TestHelper->randomDataVector<int>(10), idx));
    EXPECT_TRUE(testRW<long>(h5TestHelper->randomDataVector<long>(10), idx));
    EXPECT_TRUE(testRW<float>(h5TestHelper->randomDataVector<float>(10), idx));
    EXPECT_TRUE(testRW<double>(h5TestHelper->randomDataVector<double>(10), idx));
    EXPECT_TRUE(testRW<char>(GenH5::Vector<char>{'C', '\n', '#', '\0'}, idx));
    // will result in false negative due to comparing pointers
//    EXPECT_TRUE(testRW(QVector<char const*>{"Abc\0", "Def\0", "Geh\0",
//                                            "char const*#!^\0"}, counter));
}

TEST_F(TestH5AbstractDataSet, RWstrings)
{
    int idx = 0;
    EXPECT_TRUE(testRW<QByteArray>(h5TestHelper->randomByteArrays(10), idx));
    EXPECT_TRUE(testRW<QString>(h5TestHelper->randomStringList(10), idx));
}

TEST_F(TestH5AbstractDataSet, RWcomp2D)
{
    auto origStrs = h5TestHelper->randomStringList(10);
    auto origDData = h5TestHelper->randomDataVector<double>(10);
    auto data = GenH5::makeCompData(origStrs, origDData);

    auto dset = root.createDataset(QByteArrayLiteral("test_2D"),
                                   data.dataType(),
                                   data.dataSpace());

    ASSERT_TRUE(dset.isValid());
    EXPECT_TRUE(dset.dataSpace() == data.dataSpace());
    EXPECT_TRUE(dset.dataType() == data.dataType());

    EXPECT_TRUE(dset.write(data));

    GenH5::CompData<QString, double> desData;
    EXPECT_TRUE(dset.read(desData));

    // data must be deserialized first
    QStringList strData;
    QVector<double> dData;
    desData.deserialize(strData, dData);

    EXPECT_EQ(origStrs, strData);
    EXPECT_EQ(origDData, dData);
}

TEST_F(TestH5AbstractDataSet, RWcomp3D)
{
    auto origStrs = h5TestHelper->randomStringList(10);
    auto origDData = h5TestHelper->randomDataVector<double>(10);
    auto origBaData = h5TestHelper->randomByteArrays(10);
    auto data = GenH5::makeCompData(origStrs, origDData, origBaData);

    auto dset = root.createDataset(QByteArrayLiteral("test_3D"),
                                   data.dataType(),
                                   data.dataSpace());

    ASSERT_TRUE(dset.isValid());
    EXPECT_TRUE(dset.dataSpace() == data.dataSpace());
    EXPECT_TRUE(dset.dataType() == data.dataType());

    EXPECT_TRUE(dset.write(data));

    GenH5::CompData<QString, double, QByteArray> data2;
    EXPECT_TRUE(dset.read(data2));

    // data must be deserialized first
    QStringList strData;
    QVector<double> dData;
    QVector<QByteArray> baData;
    data2.deserialize(strData, dData, baData);

    EXPECT_EQ(origStrs, strData);
    EXPECT_EQ(origDData, dData);
    EXPECT_EQ(origBaData, baData);
}

TEST_F(TestH5AbstractDataSet, RWcomplex)
{
    auto origStrs = h5TestHelper->randomStringList(5);
    // manual random points
    GenH5::Vector<QPointF> origPts1{{32.1, 42.01}, {.1, 11}, {0.2, .11}, {12.12, 12}};
    GenH5::Vector<QPointF> origPts2{};
    GenH5::Vector<QPointF> origPts3{{12.12, 12}};
    GenH5::Vector<QPointF> origPts4{{32.1, 42.01}, {.1, 11}, {0.2, .11}};
    GenH5::Vector<QPointF> origPts5{{1.1, 2.1}, {12, 12}};

    auto points = { origPts1, origPts2, origPts3, origPts4, origPts5 };

    auto data = GenH5::makeCompData(origStrs, points);

    auto dset = root.createDataset(QByteArrayLiteral("test_3D"),
                                   data.dataType({"strings", "points"}),
                                   data.dataSpace());

    ASSERT_TRUE(dset.isValid());
    EXPECT_TRUE(dset.dataSpace() == data.dataSpace());
    EXPECT_TRUE(dset.dataType() == data.dataType());

    qDebug() << "writing to..." << file.filePath();
    EXPECT_TRUE(dset.write(data));

    GenH5::CompData<QString, GenH5::VarLen<QPointF>> desData;
    EXPECT_TRUE(dset.read(desData));

    // data must be deserialized first
    QStringList strData;
    QVector<GenH5::VarLen<QPointF>> ptsData;
    desData.deserialize(strData, ptsData);

    ASSERT_EQ(ptsData.size(), origStrs.size());
    ASSERT_EQ(strData.size(), origStrs.size());

    EXPECT_TRUE(std::equal(std::cbegin(ptsData), std::cend(ptsData),
                           std::cbegin(points)));
}

#if 0 // test hvl_t workings
static QByteArray filePath;

TEST_F(TestH5AbstractDataSet, h5VarLenWrite)
{
    QVector<double> list{2.0, 1.4, 3.1, 42};
    GenH5::Data<double> data{list};

    filePath = h5TestHelper->newFilePath();

    GenH5::File file(filePath, GenH5::CreateOnly);

    auto dset = file.root().createDataset(QByteArray{"vlen_test.h5"},
                              GenH5::DataType::varLen(data.dataType()),
                              GenH5::DataSpace::Scalar);

    ASSERT_TRUE(dset.isValid());

    hvl_t vlen;
    vlen.p = data.dataPtr();
    vlen.len = data.length();

    ASSERT_TRUE(dset.write(&vlen));
}

TEST_F(TestH5AbstractDataSet, h5VarLenRead)
{
    GenH5::File file(filePath, GenH5::OpenOnly);

    auto dset = file.root().openDataset(QByteArray{"vlen_test.h5"});

    ASSERT_TRUE(dset.isValid());

    hvl_t vlen{};

    qDebug() << vlen.len << vlen.p;
    ASSERT_TRUE(dset.read(&vlen));
    qDebug() << vlen.len << vlen.p;

    qDebug() << vlen.p;
}
#endif

TEST_F(TestH5AbstractDataSet, RWmultiDim)
{
    GenH5::DataSet dset = root.createDataset(QByteArrayLiteral("multidim_test"),
                                          doubleData.dataType(),
                                          GenH5::DataSpace({3, 2}));
    ASSERT_TRUE(dset.isValid());

    // write 1D data as 2D
    EXPECT_TRUE(dset.write(doubleData));

    // read 2D data as 1D
    GenH5::Data<double> readData;
    EXPECT_TRUE(dset.read(readData));

    // compare data
    EXPECT_EQ(readData.c(), doubleData.c());

    // compare datatype and dataspace
    EXPECT_TRUE(dset.dataType()  == doubleData.dataType());
    EXPECT_TRUE(dset.dataSpace() == GenH5::DataSpace({3, 2}));
    EXPECT_TRUE(dset.dataSpace().size() == doubleData.length());
}
