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
        emptyIntData  = QVector<int>();
        intData    = QVector<int>{1, 2, 3, 4, 5, 6};
        doubleData = QVector<double>{1, 2, 3, 4, 5, 6};
        stringData = QVector<QString>{"1", "2", "3", "4", "5", "6"};

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

    GenH5::Data<int> emptyIntData;
    GenH5::Data<int> intData;
    GenH5::Data<double> doubleData;
    GenH5::Data<QString> stringData;

    template<typename T, typename TContainer>
    bool testRW(TContainer const& values)
    {
        auto name = typeid(T).name();
        // for easier debugging
        qDebug() << __FUNCTION__ << name;

        GenH5::Data<T> data{values};

        // create new dataset
        auto dset = root.createDataset(QByteArrayLiteral("test_") +
                                       QByteArray(name),
                                       data.dataType(),
                                       data.dataSpace());
        EXPECT_TRUE(dset.dataSpace() == data.dataSpace());
        EXPECT_TRUE(dset.dataType() == data.dataType());
        bool success = dset.dataSpace() == data.dataSpace() &&
                       dset.dataType() == data.dataType();

        // write data
        success &= dset.write(data);
        EXPECT_TRUE(success);

        // read data
        GenH5::Data<T> readData;
        success &= dset.read(readData);
        EXPECT_TRUE(success);

        // test deserialize
        auto desValues = readData.template values<TContainer>();
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

TEST_F(TestH5AbstractDataSet, write)
{
    /** using pointer **/
    EXPECT_TRUE(dataset.write(intData.data()));
    // false because of nullptr
    EXPECT_FALSE(dataset.write(nullptr));

    /** using vector **/
    EXPECT_TRUE(dataset.write(intData.raw()));
    // sizes differ
    EXPECT_FALSE(dataset.write(emptyIntData.raw()));
    // dtype can be converted and sizes are equal
    EXPECT_TRUE(dataset.write(doubleData.raw()));
    // dtype cannot be converted
    qDebug() << "### EXPECTING ERROR: No conversion path";
    EXPECT_THROW(dataset.write(stringData.raw(), stringData.dataType()),
                 GenH5::DataSetException);
    qDebug() << "### END";

    /** using data vector **/
    EXPECT_TRUE(dataset.write(intData));
    // sizes differ
    EXPECT_FALSE(dataset.write(emptyIntData));
    // dtype can be converted
    EXPECT_TRUE(dataset.write(doubleData));
    // dtype cannot be converted
    qDebug() << "### EXPECTING ERROR: No conversion path";
    EXPECT_THROW(dataset.write(stringData), GenH5::DataSetException);
    qDebug() << "### END";
}

TEST_F(TestH5AbstractDataSet, read)
{
    /** using pointer **/
    EXPECT_TRUE(dataset.read(intData.data()));
    // false because of nullptr
    EXPECT_FALSE(dataset.read(nullptr));

    /** using vector **/
    EXPECT_TRUE(dataset.read(intData.raw()));
    // data will be resized
    EXPECT_TRUE(dataset.read(emptyIntData.raw()));
    // dtype can be converted and data will be resized
    EXPECT_TRUE(dataset.read(doubleData.raw()));
    // dtype cannot be converted
    qDebug() << "### EXPECTING ERROR: No conversion path";
    EXPECT_THROW(dataset.read(stringData.raw(), stringData.dataType()),
                 GenH5::DataSetException);
    qDebug() << "### END";

    /** using data vector **/
    EXPECT_TRUE(dataset.read(intData));
    // data will be resized
    EXPECT_TRUE(dataset.read(emptyIntData));
    // dtype can be converted and data will be resized
    EXPECT_TRUE(dataset.read(doubleData));
    // dtype cannot be converted
    qDebug() << "### EXPECTING ERROR: No conversion path";
    EXPECT_THROW(dataset.read(stringData), GenH5::DataSetException);
    qDebug() << "### END";
}

TEST_F(TestH5AbstractDataSet, writeScalar)
{
    QString d = "hello world";
    GenH5::Data0D<QString> data{d};

    auto dset = root.createDataset("string_scalar",
                                   data.dataType(),
                                   GenH5::DataSpace::Scalar);

    EXPECT_TRUE(dset.write(data));

    data = {};
    EXPECT_TRUE(dset.read(data));
    EXPECT_EQ(data.value(), QString{d});
}

TEST_F(TestH5AbstractDataSet, writeNull)
{
    GenH5::Data0D<double> data{42.123};
    auto dset = root.createDataset("double_null",
                                   data.dataType(),
                                   GenH5::DataSpace::Null);

    EXPECT_FALSE(dset.write(data));

    // no data will have been written
    data = {};
    EXPECT_FALSE(dset.read(data));
    EXPECT_EQ(data.value(), 0);
}

TEST_F(TestH5AbstractDataSet, writeTooFewElements)
{
    // when data.length != dataspace.sum
    // to few elements
    QVector<int> dataShort{intData.mid(0, 3)};
    // writing less than needed is not allowed
    EXPECT_FALSE(dataset.write(dataShort));
}

TEST_F(TestH5AbstractDataSet, writeTooManyElements)
{
    // when data.length != dataspace.sum
    // to many elements
    GenH5::Vector<int> dataLong = intData.mid(0, 3) + intData;

    // writing more than needed is technically allowed
    EXPECT_TRUE(dataset.write(dataLong));
    // but only the first entries were actually written
    GenH5::Data<int> readData;
    EXPECT_TRUE(dataset.read(readData));
    EXPECT_EQ(readData.values(), dataLong.mid(0, intData.length()));
}

TEST_F(TestH5AbstractDataSet, RWsimple1D)
{
    EXPECT_TRUE(testRW<int>(h5TestHelper->randomDataVector<int>(10)));
    EXPECT_TRUE(testRW<long>(h5TestHelper->randomDataVector<long>(10)));
    EXPECT_TRUE(testRW<float>(h5TestHelper->randomDataVector<float>(10)));
    EXPECT_TRUE(testRW<double>(h5TestHelper->randomDataVector<double>(10)));
    EXPECT_TRUE(testRW<char>(GenH5::Vector<char>{'C', '\n', '#', '\0'}));
}

TEST_F(TestH5AbstractDataSet, RWstrings1D)
{
    EXPECT_TRUE(testRW<QByteArray>(h5TestHelper->randomByteArrays(10)));
    EXPECT_TRUE(testRW<QString>(h5TestHelper->randomStringList(10)));
}

TEST_F(TestH5AbstractDataSet, RWsimple2D)
{
    GenH5::DataSet dset = root.createDataset("test_2D",
                                             doubleData.dataType(),
                                             GenH5::DataSpace({3, 2}));

    // write 1D data as 2D
    EXPECT_TRUE(dset.write(doubleData));

    // read 2D data as 1D
    GenH5::Data<double> readData;
    EXPECT_TRUE(dset.read(readData));

    // compare data
    EXPECT_EQ(readData.raw(), doubleData.raw());

    // compare datatype and dataspace
    EXPECT_TRUE(dset.dataType()  == doubleData.dataType());
    EXPECT_TRUE(dset.dataSpace() == GenH5::DataSpace({3, 2}));
    EXPECT_TRUE(dset.dataSpace().size<int>() == doubleData.length());
}

TEST_F(TestH5AbstractDataSet, RWcompound)
{
    auto origStrs = h5TestHelper->randomStringList(10);
    auto origDData = h5TestHelper->randomDataVector<double>(10);
    auto origBaData = h5TestHelper->randomByteArrays(10);

    auto data = GenH5::makeCompData(origStrs, origDData, origBaData);
    // set compound type names
    data.setTypeNames({"strings", "doubles", "bytearrays"});

    auto dset = root.createAttribute("string, double, bytearray",
                                     data.dataType(),
                                     data.dataSpace());

    EXPECT_TRUE(dset.write(data));

    GenH5::CompData<QString, double, QByteArray> data2;
    data2.setTypeNames(dset.dataType());
    EXPECT_TRUE(dset.read(data2));

    EXPECT_EQ((data2.getValues<0, QStringList>()), origStrs);
    EXPECT_EQ(data2.getValues<1>(), origDData);
    EXPECT_EQ(data2.getValues<2>(), origBaData);
}

TEST_F(TestH5AbstractDataSet, RWcomplex)
{
    auto origStrs = h5TestHelper->randomStringList(5);
    // manual random points
    GenH5::Vector<QPointF> origPts1{{32.1, 42.01}, {.1, 11}, {0.2, .11},
                                    {12.12, 12}};
    GenH5::Vector<QPointF> origPts2{};
    GenH5::Vector<QPointF> origPts3{{12.12, 12}};
    GenH5::Vector<QPointF> origPts4{{32.1, 42.01}, {.1, 11}, {0.2, .11}};
    GenH5::Vector<QPointF> origPts5{{1.1, 2.1}, {12, 12}};

    auto points = { origPts1, origPts2, origPts3, origPts4, origPts5 };

    auto data = GenH5::makeCompData(origStrs, points);
    data.setTypeNames({"strings", "varlen_poins"});

    auto dset = root.createDataset("string, varlen<point>",
                                   data.dataType(),
                                   data.dataSpace());

    EXPECT_TRUE(dset.write(data));

    GenH5::CompData<QString, GenH5::VarLen<QPointF>> data2;
    data2.setTypeNames(dset.dataType());
    EXPECT_TRUE(dset.read(data2));

    // data must be deserialized first
    auto strData = data2.getValues<0, QStringList>();
    auto ptsData = data2.getValues<1>();

    ASSERT_EQ(ptsData, points);
    ASSERT_EQ(strData, origStrs);
}

TEST_F(TestH5AbstractDataSet, RWarrayAsSimple)
{
    using GenH5::Array;
    using GenH5::Vector;

    Array<QString, 3> d1 = {"Hello world", "A", "ABC"};
    Array<QString, 3> d2 = {"Test", "", "2"};
    Array<QString, 3> d3 = {"Fancy String", "Not so long text", "!?!"};
    Array<QString, 3> d4 = {"My String", "DEF", "42"};

    GenH5::Data<QString[3]> data{d1, d2, d3, d4};

    ASSERT_TRUE(data.dataType().isArray());

    auto dset = root.createDataset("string[3]",
                                   data.dataType(),
                                   data.dataSpace());

    EXPECT_TRUE(dset.write(data));

    // read array dataset as is were a list of strings
    GenH5::Data<QString> data2;
    dset.read(data2, data.dataType());

    ASSERT_EQ(data2.size(), data.size() * 3);

    // split list at every third entry
    Vector<Vector<QString>> list = data2.split(3);
    EXPECT_EQ(list.size(), data.size());
}

TEST_F(TestH5AbstractDataSet, RWcompoundArrayAsSimple)
{
    using GenH5::Comp;
    using GenH5::Array;
    using GenH5::Vector;

    Array<QString, 3> d1 = {"Hello world", "A", "ABC"};
    Array<QString, 3> d2 = {"Test", "", "2"};
    Array<QString, 3> d3 = {"Fancy String", "Not so long text", "!?!"};
    Array<QString, 3> d4 = {"My String", "DEF", "42"};

    GenH5::CompData<QString[3]> data{d1, d2, d3, d4};

    ASSERT_TRUE(data.dataType().compoundMembers().first().type.isArray());

    auto dset = root.createDataset("comp_string[3]",
                                   data.dataType(),
                                   data.dataSpace());

    EXPECT_TRUE(dset.write(data));

    // read array dataset as is were a list of strings
    GenH5::CompData<QString> data2;
    dset.read(data2, data.dataType());

    ASSERT_EQ(data2.size(), data.size() * 3);

    // split list at every third entry
    Vector<Vector<Comp<QString>>> list = data2.split(3);
    EXPECT_EQ(list.size(), data.size());
}

#if 0 // test hvl_t workings
static QByteArray filePath;

TEST_F(TestH5AbstractDataSet, h5VarLenWrite)
{
    QVector<double> list{2.0, 1.4, 3.1, 42};
    GenH5::Data<double> data{list};

    filePath = h5TestHelper->newFilePath();

    GenH5::File file(filePath, GenH5::Create);

    auto dset = file.root().createDataset(QByteArray{"vlen_test.h5"},
                              GenH5::DataType::varLen(data.dataType()),
                              GenH5::DataSpace::Scalar);

    ASSERT_TRUE(dset.isValid());

    hvl_t vlen;
    vlen.p = data.data();
    vlen.len = data.length();

    ASSERT_TRUE(dset.write(&vlen));
}

TEST_F(TestH5AbstractDataSet, h5VarLenRead)
{
    GenH5::File file(filePath, GenH5::Open);

    auto dset = file.root().openDataset(QByteArray{"vlen_test.h5"});

    ASSERT_TRUE(dset.isValid());

    hvl_t vlen{};

    qDebug() << vlen.len << vlen.p;
    ASSERT_TRUE(dset.read(&vlen));
    qDebug() << vlen.len << vlen.p;

    qDebug() << vlen.p;
}
#endif
