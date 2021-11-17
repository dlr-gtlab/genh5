/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5data.h"
#include "gt_h5compounddata.h"


/// This is a test fixture that does a init for each test
class TestH5Data : public testing::Test
{
protected:
    virtual void SetUp()
    {
        intData    = {1, 2, 3, 4, 5};
        doubleData = {1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5"};
    }

    QVector<int> intData;
    QVector<double> doubleData;
    QStringList stringData;
};

TEST_F(TestH5Data, isImplemented)
{
    // simple datatypes
    EXPECT_TRUE(GtH5Data<int>::isImplemented::value);
    EXPECT_TRUE(GtH5Data<long>::isImplemented::value);
    EXPECT_TRUE(GtH5Data<float>::isImplemented::value);
    EXPECT_TRUE(GtH5Data<double>::isImplemented::value);
    EXPECT_TRUE(GtH5Data<const char*>::isImplemented::value);
    EXPECT_TRUE(GtH5Data<QString>::isImplemented::value);
    EXPECT_TRUE(GtH5Data<QByteArray>::isImplemented::value);

    // bool does not have a hdf5 type associated yet
    EXPECT_FALSE(GtH5Data<bool>::isImplemented::value);

    // comp datatypes
    EXPECT_TRUE((GtH5Data<double, QString>::isImplemented::value));
    EXPECT_TRUE((GtH5Data<double, int, QString>::isImplemented::value));

    // wrapper does not support more than 3 elements in comp type
    EXPECT_FALSE((GtH5Data<double, QString, const char*, int>
                  ::isImplemented::value));
}

TEST_F(TestH5Data, isValid)
{
    // test simple types
    EXPECT_TRUE(GtH5Data<int>().dataType().isValid());
    EXPECT_TRUE(GtH5Data<long>().dataType().isValid());
    EXPECT_TRUE(GtH5Data<float>().dataType().isValid());
    EXPECT_TRUE(GtH5Data<double>().dataType().isValid());
    EXPECT_TRUE(GtH5Data<const char*>().dataType().isValid());
    EXPECT_TRUE(GtH5Data<QString>().dataType().isValid());
    EXPECT_TRUE(GtH5Data<QByteArray>().dataType().isValid());

    // test compound types
    EXPECT_TRUE((GtH5Data<int, QString>().dataType().isValid()));
    EXPECT_TRUE((GtH5Data<double, float>().dataType().isValid()));
    EXPECT_TRUE((GtH5Data<QString, double, float>().dataType().isValid()));
    EXPECT_TRUE((GtH5Data<QString, QString, double>().dataType().isValid()));
    EXPECT_TRUE((GtH5Data<int, long, const char*>().dataType().isValid()));

    // more than four members are not supported
    EXPECT_FALSE((GtH5Data<QString, double, const char*, long>()
                  .dataType().isValid()));
}

TEST_F(TestH5Data, serialization)
{
    GtH5Data<int> iData(intData);
    EXPECT_EQ(iData.data(), intData);

    GtH5Data<double> dData(doubleData);
    EXPECT_EQ(dData.data(), doubleData);

    // cannot be compared directly as stringlist is serialized to const char*
    GtH5Data<QString> strData(stringData);
    // data must be deserialized first
    QStringList orgStrData;
    strData.deserialize(orgStrData);
    EXPECT_EQ(orgStrData, stringData);

    GtH5Data<QString, int, double> comData(stringData, intData, doubleData);
    // data must be deserialized first
    QStringList orgComStrData;
    QVector<int> orgIntData;
    QVector<double> orgDoubleData;
    comData.deserialize(orgComStrData, orgIntData, orgDoubleData);

    EXPECT_EQ(orgComStrData, stringData);
    EXPECT_EQ(orgIntData, intData);
    EXPECT_EQ(orgDoubleData, doubleData);
}
