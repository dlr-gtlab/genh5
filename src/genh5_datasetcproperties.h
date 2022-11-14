/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATASETCPROPERTIES_H
#define GENH5_DATASETCPROPERTIES_H

#include "genh5_object.h"
#include "genh5_utils.h"

namespace GenH5
{
// forward decl
class DataSpace;

class GENH5_EXPORT DataSetCProperties : public Object
{
public:

    static Dimensions autoChunk(DataSpace const& dataspace) noexcept;

    static DataSetCProperties autoChunked(DataSpace const& dataspace,
                                          int compression = 0) noexcept(false)
    {
        return DataSetCProperties{autoChunk(dataspace), compression};
    }

    explicit DataSetCProperties() noexcept;
    explicit DataSetCProperties(H5::DSetCreatPropList properties);
    explicit DataSetCProperties(Dimensions const& chunkDimensions,
                                int compression = 0) noexcept(false);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DSetCreatPropList const& toH5() const noexcept;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief setChunkDimensions
     * @param dimensions chunk dimensions
     */
    void setChunkDimensions(Dimensions const& dimensions) noexcept(false);

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    /**
     * @brief sets the compressions (deflate). Must be within 0-9. Can only
     * be used once the chunk dimensions are set.
     * @param level compression level between 0 (none) and 9 (max)
     */
    [[deprecated("use setDeflate instead")]]
    void setCompression(int level) noexcept(false);
#endif

    /**
     * @brief sets the compressions (gzip deflate). Must be within 0-9. Can only
     * be used once the chunk dimensions are set.
     * @param level compression level between 0 (none) and 9 (max)
     */
    void setDeflate(int level) noexcept(false);

    /**
     * @brief isChunked
     * @return whether chunking is set
     */
    bool isChunked() const noexcept;

    /**
     * @brief Whether compression is enabled (using gzip deflate)
     * @return is compressed
     */
    bool isDeflated() const noexcept;

    /**
     * @brief The level of the deflation. Level is between 0 (none) and 9 (max)
     * @return deflate level. Returns 0 if no deflate compression was used
     */
    int deflation() const noexcept;

    /**
     * @brief chunkDimensions
     * @return chunk dimensions
     */
    Dimensions chunkDimensions() const noexcept;

private:

    /// hdf5 base instance
    H5::DSetCreatPropList m_properties{};
};

} // namespace GenH5

#endif // GENH5_DATASETCPROPERTIES_H
