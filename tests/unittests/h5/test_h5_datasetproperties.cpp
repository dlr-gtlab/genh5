/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_datasetproperties.h"
#include "gth5_dataspace.h"

/// This is a test fixture that does a init for each test
class TestH5DataSetProperties : public testing::Test
{
protected:

    int maxChunks{10};
    QVector<uint64_t> chunkDims{5, 2};
    GtH5DataSetProperties propDefault{};
    GtH5DataSetProperties propChunked{chunkDims, 0};
    GtH5DataSetProperties propCompressed{chunkDims, 9};
};

TEST_F(TestH5DataSetProperties, isValid)
{
    // all instances should be valid
    EXPECT_TRUE(propDefault.isValid());
    EXPECT_TRUE(propChunked.isValid());
    EXPECT_TRUE(propCompressed.isValid());
}

TEST_F(TestH5DataSetProperties, chunking)
{
    EXPECT_FALSE(propDefault.isChunked());
    EXPECT_TRUE(propChunked.isChunked());
    EXPECT_TRUE(propCompressed.isChunked());

    EXPECT_EQ(propDefault.chunkDimensions(), QVector<uint64_t>());
    EXPECT_EQ(propChunked.chunkDimensions(), chunkDims);
    EXPECT_EQ(propCompressed.chunkDimensions(), chunkDims);
}

TEST_F(TestH5DataSetProperties, autoChunk)
{
    GtH5DataSpace dspaceNull;
    EXPECT_EQ(GtH5DataSetProperties::autoChunk(dspaceNull, maxChunks),
              QVector<uint64_t>());

    // chunk dim may not be zero
    GtH5DataSpace dspaceZero(0);
    EXPECT_EQ(GtH5DataSetProperties::autoChunk(dspaceZero, maxChunks),
              QVector<uint64_t>{1});
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
