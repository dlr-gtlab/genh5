/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATASETCPROPERTIES_H
#define GTH5_DATASETCPROPERTIES_H

#include "gth5_object.h"
#include "gth5_utils.h"

namespace GtH5
{
// forward decl
class DataSpace;

class GTH5_EXPORT DataSetCProperties : public Object
{
public:

    static Dimensions autoChunk(DataSpace const& dataspace);
    static DataSetCProperties autoChunked(DataSpace const& dataspace,
                                          int compression = 0);

    DataSetCProperties();
    explicit DataSetCProperties(H5::DSetCreatPropList properties);
    explicit DataSetCProperties(Dimensions const& chunkDimensions,
                                int compression = 0);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DSetCreatPropList const& toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const override;

    /**
     * @brief setChunkDimensions
     * @param dimensions chunk dimensions
     */
    void setChunkDimensions(Dimensions const& dimensions);

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
    Dimensions chunkDimensions() const;

private:

    /// hdf5 base instance
    H5::DSetCreatPropList m_properties{};
};

inline DataSetCProperties
DataSetCProperties::autoChunked(DataSpace const& dataspace,
                                int compression)
{
    return DataSetCProperties{DataSetCProperties::autoChunk(dataspace),
                              compression};
}

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5DataSetProperties = GtH5::DataSetCProperties;
#endif

GTH5_EXPORT std::ostream& operator<<(std::ostream& s,
                                     GtH5::DataSetCProperties const& d);
GTH5_EXPORT QDebug operator<<(QDebug s, GtH5::DataSetCProperties const& d);

#endif // GTH5_DATASETCPROPERTIES_H
