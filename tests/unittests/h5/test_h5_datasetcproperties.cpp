/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "genh5_datasetcproperties.h"
#include "genh5_dataset.h"
#include "genh5_file.h"

#include "testhelper.h"

#include "H5Zpublic.h"

/// This is a test fixture that does a init for each test
class TestH5DataSetCProperties : public testing::Test
{
protected:

//    int maxChunks{10};
    GenH5::Dimensions chunkDims{5, 2};
    GenH5::DataSetCProperties propDefault{};
    GenH5::DataSetCProperties propChunked{chunkDims, 0};
    GenH5::DataSetCProperties propCompressed{chunkDims, 9};
};

TEST_F(TestH5DataSetCProperties, isValid)
{
    // all instances should be valid
    EXPECT_TRUE(propDefault.isValid());
    EXPECT_TRUE(propChunked.isValid());
    EXPECT_TRUE(propCompressed.isValid());
}

TEST_F(TestH5DataSetCProperties, chunking)
{
    EXPECT_FALSE(propDefault.isChunked());
    EXPECT_TRUE(propChunked.isChunked());
    EXPECT_TRUE(propCompressed.isChunked());

    EXPECT_EQ(propDefault.chunkDimensions(), GenH5::Dimensions{});
    EXPECT_EQ(propChunked.chunkDimensions(), chunkDims);
    EXPECT_EQ(propCompressed.chunkDimensions(), chunkDims);
}

TEST_F(TestH5DataSetCProperties, autoChunk)
{
    GenH5::DataSpace dspaceNull{};
    EXPECT_EQ(GenH5::DataSetCProperties::autoChunk(dspaceNull),
              GenH5::Dimensions{});

    // chunk dim may not be zero
    GenH5::DataSpace dspaceZero{0};
    EXPECT_EQ(GenH5::DataSetCProperties::autoChunk(dspaceZero),
              GenH5::Dimensions{1});
}

TEST_F(TestH5DataSetCProperties, compression)
{
    EXPECT_FALSE(propDefault.isDeflated());
    EXPECT_FALSE(propChunked.isDeflated());
    EXPECT_TRUE(propCompressed.isDeflated());

    qDebug() << "### EXPECTING ERROR: Filter not in the pipeline";
    EXPECT_EQ(propDefault.deflation(), 0);
    qDebug() << "### END";
    qDebug() << "### EXPECTING ERROR: Filter not in the pipeline";
    EXPECT_EQ(propChunked.deflation(), 0);
    qDebug() << "### END";
    EXPECT_EQ(propCompressed.deflation(), 9);
}

TEST_F(TestH5DataSetCProperties, hasDeflateFilter)
{
    bool isAvail = H5Zfilter_avail(H5Z_FILTER_DEFLATE);
    qDebug() << "HAS DEFLATE:" << isAvail;
//    EXPECT_TRUE(isAvail);
}

TEST_F(TestH5DataSetCProperties, createDataSetOptionalParam)
{
    GenH5::File file{h5TestHelper->newFilePath(), GenH5::Create};
    ASSERT_TRUE(file.isValid());

    // optional was explicitly created -> not default and not chunked
    auto dset1 = file.root().createDataSet(QByteArrayLiteral("testA"),
                                           GenH5::DataType::Float,
                                           GenH5::DataSpace::linear(10),
                                           GenH5::DataSetCProperties{});

    EXPECT_FALSE(dset1.cProperties().isChunked());

    // optional is default -> by default dset will be chunked
    auto dset2 = file.root().createDataSet(
                                QByteArrayLiteral("testB"),
                                GenH5::DataType::VarString,
                                GenH5::DataSpace::linear(10),
                                GenH5::Optional<GenH5::DataSetCProperties>{});

    EXPECT_TRUE(dset2.cProperties().isChunked());
}
