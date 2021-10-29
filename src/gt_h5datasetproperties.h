/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATASETPROPERTIES_H
#define GTH5DATASETPROPERTIES_H

#include "gt_h5object.h"
#include <QVector>

class GtH5DataSpace;
class GT_H5_EXPORT GtH5DataSetProperties : public GtH5Object
{
public:

    static QVector<uint64_t> autoChunk(const GtH5DataSpace& dataspace,
                                       int maxChunks = 50);

    GtH5DataSetProperties();
    GtH5DataSetProperties(const H5::DSetCreatPropList& properties);
    GtH5DataSetProperties(const QVector<uint64_t>& chunkDimensions,
                          int compression = 0);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DSetCreatPropList toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    int64_t id() const override;

    /**
     * @brief setChunkDimensions
     * @param dimensions
     */
    void setChunkDimensions(const QVector<uint64_t>& dimensions);

    /**
     * @brief sets the compressions. Must be within 0-9. Can only be used once
     * chunk dimensions are set.
     * @param compression level between 0 (none) and 9 (max)
     */
    void setCompression(int level);

    /**
     * @brief isChunked
     * @return whether chunking is set
     */
    bool isChunked() const;

    /**
     * @brief chunkDimensions
     * @return chunk dimensions
     */
    QVector<uint64_t> chunkDimensions() const;

private:

    /// hdf5 base instance
    H5::DSetCreatPropList m_properties;
};

#endif // GTH5DATASETPROPERTIES_H
