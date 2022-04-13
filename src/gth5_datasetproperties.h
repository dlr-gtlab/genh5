/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATASETPROPERTIES_H
#define GTH5DATASETPROPERTIES_H

#include "gth5_object.h"
#include <QVector>

class GtH5DataSpace;
class GTH5_EXPORT GtH5DataSetProperties : public GtH5Object
{
public:

    static QVector<uint64_t> autoChunk(GtH5DataSpace const& dataspace,
                                       int maxChunks = 50);

    GtH5DataSetProperties();
    explicit GtH5DataSetProperties(H5::DSetCreatPropList const& properties);
    explicit GtH5DataSetProperties(QVector<uint64_t> const& chunkDimensions,
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
     * @param dimensions chunk dimensions
     */
    void setChunkDimensions(QVector<uint64_t> const& dimensions);

    /**
     * @brief sets the compressions. Must be within 0-9. Can only be used once
     * chunk dimensions are set.
     * @param level compression level between 0 (none) and 9 (max)
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
    H5::DSetCreatPropList m_properties{};
};

#endif // GTH5DATASETPROPERTIES_H
