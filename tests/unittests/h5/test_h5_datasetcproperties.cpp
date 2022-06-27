/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_datasetcproperties.h"
#include "gth5_dataset.h"
#include "gth5_file.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5DataSetCProperties : public testing::Test
{
protected:

//    int maxChunks{10};
    GtH5::Dimensions chunkDims{5, 2};
    GtH5::DataSetCProperties propDefault{};
    GtH5::DataSetCProperties propChunked{chunkDims, 0};
    GtH5::DataSetCProperties propCompressed{chunkDims, 9};
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

    EXPECT_EQ(propDefault.chunkDimensions(), GtH5::Dimensions{});
    EXPECT_EQ(propChunked.chunkDimensions(), chunkDims);
    EXPECT_EQ(propCompressed.chunkDimensions(), chunkDims);
}

TEST_F(TestH5DataSetCProperties, autoChunk)
{
    GtH5::DataSpace dspaceNull{};
    EXPECT_EQ(GtH5::DataSetCProperties::autoChunk(dspaceNull),
              GtH5::Dimensions{});

    // chunk dim may not be zero
    GtH5::DataSpace dspaceZero{0};
    EXPECT_EQ(GtH5::DataSetCProperties::autoChunk(dspaceZero),
              GtH5::Dimensions{1});
}

/* not implemented yet
TEST_F(TestH5DataSetProperties, compression)
{
    EXPECT_FALSE(propDefault.isCompressed());
    EXPECT_FALSE(propChunked.isCompressed());

    EXPECT_TRUE(m_propCompressed.isCompressed());

    EXPECT_EQ(m_propDefault.compression(), 0);
    EXPECT_EQ(m_propChunked.compression(), 0);
    EXPECT_EQ(m_propCompressed.compression(), 9);
}
*/

TEST_F(TestH5DataSetCProperties, createDatasetOptionalParam)
{
    GtH5::File file{h5TestHelper->newFilePath(), GtH5::Create};
    ASSERT_TRUE(file.isValid());

    // optional was explicitly created -> not default and not chunked
    auto dset1 = file.root().createDataset(QByteArrayLiteral("testA"),
                                           GtH5::DataType::Float,
                                           GtH5::DataSpace::linear(10),
                                           GtH5::DataSetCProperties{});

    EXPECT_FALSE(dset1.properties().isChunked());

    // optional is default -> by default dset will be chunked
    auto dset2 = file.root().createDataset(
                                    QByteArrayLiteral("testB"),
                                    GtH5::DataType::Float,
                                    GtH5::DataSpace::linear(10),
                                    GtH5::Optional<GtH5::DataSetCProperties>{});

    EXPECT_TRUE(dset2.properties().isChunked());
}
