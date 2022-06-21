/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_dataset.h"
#include "gth5_group.h"
#include "gth5_file.h"
#include "gth5_data.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5DataSet : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        doubleData = GtH5::Vector<double>{1, 2, 3, 4, 5};
        intData    = GtH5::Vector<int>{1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5", "6"};

        file = GtH5::File(h5TestHelper->newFilePath(), GtH5::CreateOnly);
        ASSERT_TRUE(file.isValid());

        group = file.root().createGroup(QByteArrayLiteral("group"));
        ASSERT_TRUE(group.isValid());
    }

    GtH5::File file;
    GtH5::Group group;

    GtH5::Data<int> intData;
    GtH5::Data<double> doubleData;
    GtH5::Data<QString> stringData;
};

TEST_F(TestH5DataSet, isValid)
{
    GtH5::DataSet dset;
    // test invalid dataset
    EXPECT_FALSE(dset.isValid());

    // create and test various datasets
    dset = file.root().createDataset(QByteArrayLiteral("test"),
                                     intData.dataType(),
                                     intData.dataSpace());
    EXPECT_TRUE(dset.isValid());

    dset = group.createDataset(QByteArrayLiteral("test"),
                               doubleData.dataType(),
                               doubleData.dataSpace());
    EXPECT_TRUE(dset.isValid());
}

TEST_F(TestH5DataSet, deleteLink)
{
    GtH5::DataSet dset;
    // test invalid attribute
    EXPECT_FALSE(dset.isValid());

    // create valid attribute
    dset = file.root().createDataset(QByteArrayLiteral("test"),
                                     intData.dataType(),
                                     intData.dataSpace());
    EXPECT_TRUE(dset.isValid());

    // delete dataset
    dset.deleteLink();
    EXPECT_FALSE(dset.isValid());
}

TEST_F(TestH5DataSet, resize)
{
    // create new dataset
    GtH5::DataSet dset = group.createDataset(QByteArrayLiteral("test"),
                                           doubleData.dataType(),
                                           doubleData.dataSpace());
    ASSERT_TRUE(dset.isValid());

    // resize to zero
    EXPECT_TRUE(dset.resize({0}));
    EXPECT_TRUE(dset.isValid());
    EXPECT_EQ(dset.dataSpace().size(), 0);

    // cannot extend dataset beyond original dimensions
//    EXPECT_FALSE(dset.resize({(uint64_t) (2 * doubleData.length())}));
//    EXPECT_TRUE(dset.isValid());
//    EXPECT_EQ(dset.dataSpace().sum(), 0);
}

TEST_F(TestH5DataSet, writeSelection)
{
    GtH5::Data<float> data{h5TestHelper->linearDataVector<float>(42, 1, 1)};
    GtH5::DataSpace dspace{2, 3, 7};

    ASSERT_EQ(dspace.size(), data.length());

    // create new dataset
    GtH5::DataSet dset = group.createDataset(QByteArrayLiteral("test_selec"),
                                             data.dataType(),
                                             dspace);
    ASSERT_TRUE(dset.isValid());

    /* WRITE */
    GtH5::Dimensions count{1, 1, 6};
    auto selSize = GtH5::prod(count);
    auto selection = GtH5::makeSelection(dspace, count, {1, 2, 1});
    ASSERT_EQ(selection.size(), selSize);

    dset.write(data, selection);

    /* READ */
    GtH5::Data<float> read;
    dset.read(read);

    auto size = data.length();

    GtH5::Vector<float> dummy;
    dummy.resize(size-selSize-1);
    dummy.fill(0);

    EXPECT_EQ(read.mid(size-selSize), data.mid(0, selSize));
    EXPECT_EQ(read.mid(0, size-selSize-1), dummy);
}

TEST_F(TestH5DataSet, readSelection)
{
    GtH5::Data<uint64_t> data{h5TestHelper->linearDataVector<uint64_t>(48, 1, 1)};
    GtH5::DataSpace dspace{4, 12};

    ASSERT_EQ(dspace.size(), data.length());

    // create new dataset
    GtH5::DataSet dset = group.createDataset(QByteArrayLiteral("test_selec"),
                                             data.dataType(),
                                             dspace);
    ASSERT_TRUE(dset.isValid());

    /* WRITE */
    dset.write(data, dspace);

    /* READ */
    // reads every second element
    GtH5::Dimensions count{4, 6};
    GtH5::DataSpaceSelection selection(dspace, count);
    selection.setStride({1, 2});

    auto selSize = GtH5::prod(count);

    ASSERT_EQ(selection.size(), selSize);

    GtH5::Data<uint64_t> read;
    dset.read(read, selection);

    auto dummy = h5TestHelper->linearDataVector<uint64_t>(selSize, 1, 2);
    EXPECT_EQ(read.c(), dummy);
}

#if 0
TEST_F(TestH5DataSpace, h5selection) // test selection in hdf5
{
    auto file = GtH5::File(h5TestHelper->newFilePath(), GtH5::CreateOnly);

    GtH5::Reference ref;
    constexpr uint row = 3;
    constexpr uint col = 6;

    /* WRITE */
    {
        double plaindata[row][col] = {
            {1, 2, 3, 4, 5, 6},
            {7, 8, 9, 9, 8, 7},
            {6, 5, 4, 3, 2, 1}
        };

        GtH5::Data<double> data{h5TestHelper->linearDataVector<double>(row*col, 1, 1)};

        GtH5::DataSpace dspace{row, col};

        auto dset = file.root().createDataset(QByteArray{"test_2d"},
                                              data.dataType(),
                                              dspace);
        ASSERT_TRUE(dset.isValid());

#if 0

        // file space layout
        H5::DataSpace hspace{dspace.toH5()};

        QVector<hsize_t> count{2, 5};
        QVector<hsize_t> offset{0, 1};
        QVector<hsize_t> stride{1, 1};
        QVector<hsize_t> block{1, 1};

        // file space selection
        hspace.selectHyperslab(H5S_SELECT_SET,
                               count.constData(),
                               offset.constData(),
                               stride.constData(),
                               block.constData());

        // memory layout
        QVector<hsize_t> memDims{10};
        H5::DataSpace memspace{memDims.length(), memDims.constData()};

        dset.toH5().write(plaindata,
                          H5::PredType::NATIVE_DOUBLE,
                          memspace,
                          hspace);

#elif 1

        // same as above
        dset.write(data, GtH5::makeSelection(dspace,
                                             GtH5::Dimensions{2, 5},
                                             GtH5::Dimensions{1, 1}));

#elif 1

        // writes to every second element
        dset.write(data, GtH5::makeSelection(dspace,
                                             GtH5::Dimensions{3, 3},
                                             GtH5::Dimensions{0, 0},
                                             GtH5::Dimensions{1, 2}));

#elif 1

        // write all
        dset.write(data);

#endif

        ref = dset.toReference();

        file.close();
    }

    /* READ */
    if (ref.isValid())
    {
        qDebug() << "reading...";

        file = GtH5::File(file.filePath(), GtH5::OpenOnly);

        ASSERT_TRUE(file.isValid());

        auto dset = ref.toDataSet(file);

#if 0
        for (uint i = 0; i < 2; ++i)
        {
            auto selection = GtH5::makeSelection(dset.dataSpace(),
                                                 GtH5::Dimensions{3, 3},
                                                 GtH5::Dimensions{0, i},
                                                 GtH5::Dimensions{1, 2});

            qDebug() << "selection:"
                     << selection.dimensions()
                     << selection.selectionSize();

            GtH5Data<double> partial;
            dset.read(partial, selection);

            qDebug() << "data:"
                     << partial << partial.capacity();
        }
#endif

        GtH5::Data<double> readData;
        dset.read(readData);

        for (uint r = 0; r < row; ++r)
        {
            qDebug().nospace() << "Dim " << r << ": " << readData.mid(r*col, col);
        }
    }
}
#endif

