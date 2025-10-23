/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 03.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATASETCPROPERTIES_H
#define GENH5_DATASETCPROPERTIES_H

#include "genh5_idcomponent.h"
#include "genh5_object.h"

namespace GenH5
{
// forward decl
class DataSpace;

class GENH5_EXPORT DataSetCProperties : public Object
{
public:

    /// Instantiates a new Datatype and assigns the id without incrementing it
    static DataSetCProperties fromId(hid_t id) noexcept;

    static Dimensions autoChunk(DataSpace const& dataspace) noexcept;

    static DataSetCProperties autoChunked(DataSpace const& dataspace,
                                          int compression = 0) noexcept(false)
    {
        return DataSetCProperties{autoChunk(dataspace), compression};
    }

    DataSetCProperties();
    explicit DataSetCProperties(hid_t id);
    explicit DataSetCProperties(Dimensions const& chunkDimensions,
                                int compression = 0) noexcept(false);

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

    /// swaps all members
    void swap(DataSetCProperties& other) noexcept;

private:

    /// create properties id
    IdComponent<IdType::PropertyList> m_id;
};

} // namespace GenH5

inline void
swap(GenH5::DataSetCProperties& a, GenH5::DataSetCProperties& b) noexcept
{
    a.swap(b);
}

#endif // GENH5_DATASETCPROPERTIES_H
