
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

// QPOINT
GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);
GENH5_DECLARE_IMPLICIT_CONVERSION(QPointF);

GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    struct _PointData{ int xp, yp; };

    static_assert(sizeof(QPoint) == sizeof (_PointData),
                  "Datatype does not match");

    return GenH5::DataType::compound(sizeof(_PointData), {
        {"xp", offsetof(_PointData, xp), GenH5::dataType<int>()},
        {"yp", offsetof(_PointData, yp), GenH5::dataType<int>()}
    });
};

GENH5_DECLARE_DATATYPE_IMPL(QPointF)
{
    using Tp = decltype (std::declval<QPointF>().x());
    return GenH5::dataType<Tp, Tp>({"xp", "yp"});
};

TEST_F(Tests, compound)
{
    using CompT = GenH5::Comp<QString, double>;
    using ArrayT = GenH5::Array<CompT, 4>;

    GenH5::Vector<ArrayT> vector{
        ArrayT{
                CompT{"QPoint{1,1}", 2},
                CompT{"QPoint{1,2}", 42.1},
                CompT{"QPoint{2,0}", 0.12},
                CompT{"{}", .1}
        }
    };

    auto data = GenH5::makeData(vector);

    GenH5::File file(h5TestHelper->newFilePath(), GenH5::Create);

    auto dset = file.root().createDataset(QByteArray{"comp_data.h5"},
                                          data.dataType(),
                                          data.dataSpace());

    qDebug() << file.filePath();
    ASSERT_TRUE(dset.isValid());

    ASSERT_TRUE(dset.write(data));

    GenH5::Data<ArrayT> desData;
    desData.clear();
    dset.read(desData);

    ArrayT desArray = desData.deserializeIdx(0);

    GenH5::Vector<QString> strings;
    GenH5::Vector<double>  doubles;
    GenH5::unpackNested(desArray, strings, doubles);

    qDebug() << strings;
    qDebug() << doubles;
}
