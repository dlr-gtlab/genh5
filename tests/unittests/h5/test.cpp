
#include "gtest/gtest.h"
#include "testhelper.h"

#include <QPoint>
#include <QPointF>

#include "gth5.h"
#include "gth5_data.h"

class Tests : public testing::Test
{

};

// QPOINT
GTH5_DECLARE_IMPLICIT_CONVERSION(QPoint);
GTH5_DECLARE_IMPLICIT_CONVERSION(QPointF);

GTH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    struct _PointData{ int xp, yp; };

    static_assert(sizeof(QPoint) == sizeof (_PointData),
                  "Datatype does not match");

    return GtH5::DataType::compound(sizeof(_PointData), {
        {"xp", offsetof(_PointData, xp), GtH5::dataType<int>()},
        {"yp", offsetof(_PointData, yp), GtH5::dataType<int>()}
    });
};

GTH5_DECLARE_DATATYPE_IMPL(QPointF)
{
    using Tp = decltype (std::declval<QPointF>().x());
    return GtH5::dataType<Tp, Tp>({"xp", "yp"});
};

TEST_F(Tests, compound)
{
    using CompT = GtH5::Comp<QString, double>;
    using ArrayT = GtH5::Array<CompT, 4>;

    GtH5::Vector<ArrayT> vector{
        ArrayT{
                CompT{"QPoint{1,1}", 2},
                CompT{"QPoint{1,2}", 42.1},
                CompT{"QPoint{2,0}", 0.12},
                CompT{"{}", .1}
        }
    };

    auto data = GtH5::makeData(vector);

    GtH5::File file(h5TestHelper->newFilePath(), GtH5::Create);

    auto dset = file.root().createDataset(QByteArray{"comp_data.h5"},
                                          data.dataType(),
                                          data.dataSpace());

    qDebug() << file.filePath();
    ASSERT_TRUE(dset.isValid());

    ASSERT_TRUE(dset.write(data));

    GtH5::Data<ArrayT> desData;
    desData.clear();
    dset.read(desData);

    ArrayT desArray = desData.deserializeIdx(0);

    GtH5::Vector<QString> strings;
    GtH5::Vector<double>  doubles;
    GtH5::unpackNested(desArray, strings, doubles);

    qDebug() << strings;
    qDebug() << doubles;
}
