/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 30.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_dataset.h"
#include "genh5_group.h"
#include "genh5_file.h"

#include "testhelper.h"

#include <QDebug>
#include <QStringList>

/// This is a test fixture that does a init for each test
class TestH5DataSet : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        doubleData = GenH5::Vector<double>{1, 2, 3, 4, 5};
        intData    = GenH5::Vector<int>{1, 2, 3, 4, 5};
        stringData = QStringList{"1", "2", "3", "4", "5", "6"};

        file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
        ASSERT_TRUE(file.isValid());
    }

    GenH5::File file;

    GenH5::Data<int> intData;
    GenH5::Data<double> doubleData;
    GenH5::Data<QString> stringData;
};

TEST_F(TestH5DataSet, isValid)
{
    // test invalid dataset
    GenH5::DataSet dset;
    EXPECT_FALSE(dset.isValid());
    EXPECT_THROW(dset.cProperties(), GenH5::DataSetException);

    // valid dataset
    dset = file.root().createDataSet("test",
                                     intData.dataType(),
                                     intData.dataSpace());
    EXPECT_TRUE(dset.isValid());
    EXPECT_TRUE(dset.cProperties().isValid());
}

TEST_F(TestH5DataSet, deleteLink)
{
    GenH5::DataSet dset;
    // test invalid attribute
    EXPECT_FALSE(dset.isValid());

    // create valid dataset
    dset = file.root().createDataSet("test",
                                     intData.dataType(),
                                     intData.dataSpace());
    EXPECT_TRUE(dset.isValid());
    EXPECT_TRUE(file.root().exists("test"));

    // delete dataset
    dset.deleteLink();
    EXPECT_FALSE(dset.isValid());
    EXPECT_FALSE(file.root().exists("test"));
}

TEST_F(TestH5DataSet, deleteLinkNested)
{
    qDebug() << file.filePath();

    // create nested groups
    auto dset = file.root()
            .createGroup("A")
            .createGroup("B")
            .createGroup("C")
            .createGroup("D")
            .writeDataSet("my_dset", QVector<uint>(42u));

    EXPECT_TRUE(file.root().exists("A/B/C/D/my_dset"));

    EXPECT_NO_THROW(dset.deleteLink());

    EXPECT_FALSE(file.root().exists("A/B/C/D/my_dset"));

    EXPECT_NO_THROW(file.root().openGroup("A").deleteLink());

    EXPECT_FALSE(file.root().exists("A"));
}

TEST_F(TestH5DataSet, deleteLinkRecursively)
{
    qDebug() << file.filePath();

    // create nested groups
    file.root()
            .createGroup("A")
            .createGroup("B")
            .createGroup("C")
            .createGroup("D")
            .writeDataSet("my_dset", QVector<uint>(42u));

    EXPECT_TRUE(file.root().exists("A/B/C/D/my_dset"));

    EXPECT_NO_THROW(file.root().openGroup("A").deleteRecursively());

    EXPECT_FALSE(file.root().exists("A"));
}

TEST_F(TestH5DataSet, resize)
{
    auto group = file.root().createGroup("group");
    ASSERT_TRUE(group.isValid());

    // create new dataset
    auto dset = group.createDataSet("test",
                                    doubleData.dataType(),
                                    doubleData.dataSpace());

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
    auto group = file.root().createGroup("group");
    ASSERT_TRUE(group.isValid());

    GenH5::Data<float> data{h5TestHelper->linearDataVector<float>(42, 1)};
    GenH5::DataSpace dspace{2, 3, 7};
    ASSERT_EQ(dspace.size(), data.length());

    // create new dataset
    GenH5::DataSet dset = group.createDataSet("test_selection",
                                              data.dataType(),
                                              dspace);

    /* WRITE */
    GenH5::Dimensions count{1, 1, 6};
    auto selSize = GenH5::prod<int>(count);
    auto selection = GenH5::makeSelection(dspace, count, {1, 2, 1});
    ASSERT_EQ(selection.size(), selSize);

    dset.write(data, selection);

    /* READ */
    GenH5::Data<float> read;
    dset.read(read);

    auto size = data.length();

    GenH5::Vector<float> dummy;
    dummy.resize(size-selSize-1);

    EXPECT_EQ(read.mid(size-selSize), data.mid(0, selSize));
    EXPECT_EQ(read.mid(0, size-selSize-1), dummy);
}

TEST_F(TestH5DataSet, readSelection)
{
    auto group = file.root().createGroup("group");
    ASSERT_TRUE(group.isValid());

    GenH5::Data<uint64_t> data{h5TestHelper->linearDataVector<uint64_t>(48, 1)};
    GenH5::DataSpace dspace{4, 12};
    ASSERT_EQ(dspace.size(), data.length());

    // create new dataset
    GenH5::DataSet dset = group.createDataSet("test_selec",
                                              data.dataType(),
                                              dspace);

    /* WRITE */
    dset.write(data, dspace);

    /* READ */
    // reads every second element
    GenH5::Dimensions count{4, 6};
    GenH5::DataSpaceSelection selection(dspace, count);
    selection.setStride({1, 2});

    auto selSize = GenH5::prod<int>(count);

    ASSERT_EQ(selection.size(), selSize);

    GenH5::Data<uint64_t> read;
    dset.read(read, selection);

    auto dummy = h5TestHelper->linearDataVector<uint64_t>(selSize, 1, 2);
    EXPECT_EQ(read.values(), dummy);
}

#if 0
#include "genh5_reference.h"

TEST_F(TestH5DataSet, h5selection) // test selection in hdf5
{
    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
    auto filePath = file.filePath();

    GenH5::Reference ref;
    constexpr uint row = 3;
    constexpr uint col = 6;

    /* WRITE */
    {
        qDebug() << "writing...";

        double plaindata[row][col] = {
            {1, 2, 3, 4, 5, 6},
            {7, 8, 9, 9, 8, 7},
            {6, 5, 4, 3, 2, 1}
        };

        GenH5::Data<double> data{h5TestHelper->linearDataVector<double>(row*col, 1, 1)};

        GenH5::DataSpace dspace{row, col};

        auto dset = file.root().createDataSet(QByteArray{"test_2d"},
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

#elif 0

        // same as above
        dset.write(data, GenH5::makeSelection(dspace,
                                             GenH5::Dimensions{2, 5},
                                             GenH5::Dimensions{1, 1}));

#elif 1

        // writes to every second element
        dset.write(data, GenH5::makeSelection(dspace,
                                             GenH5::Dimensions{3, 3},
                                             GenH5::Dimensions{0, 0},
                                             GenH5::Dimensions{1, 2}));

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
        qDebug() << "reading..." << filePath;

        file = GenH5::File(filePath, GenH5::Open);

        ASSERT_TRUE(file.isValid());

        auto dset = ref.toDataSet(file);

#if 0
        for (uint i = 0; i < 2; ++i)
        {
            auto selection = GenH5::makeSelection(dset.dataSpace(),
                                                 GenH5::Dimensions{3, 3},
                                                 GenH5::Dimensions{0, i},
                                                 GenH5::Dimensions{1, 2});

            qDebug() << "selection:"
                     << selection.space().dimensions()
                     << selection.space().selectionSize();

            GenH5::Data<double> partial;
            dset.read(partial, selection);

            qDebug() << "data:" << partial.raw() << partial.capacity();
        }
#endif

        GenH5::Data<double> readData;
        dset.read(readData);

        for (uint r = 0; r < row; ++r)
        {
            qDebug().nospace() << "Dim " << r << ": " << readData.mid(r*col, col);
        }
    }
}
#endif

