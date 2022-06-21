/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_DATASPACE_H
#define GTH5_DATASPACE_H

#include "gth5_object.h"
#include "gth5_utils.h"

namespace GtH5
{

/**
 * @brief The DataSpace class
 */
class GTH5_EXPORT DataSpace : public Object
{
public:

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
    template<typename Tin, typename Tout = int>
    static Tout sum(QVector<Tin> const& dimensions)
    {
        return GtH5::prod<Tout>(dimensions);
    }

    template<typename Tout = int>
    Tout sum() const
    {
        return GtH5::prod<Tout>(dimensions());
    }
#endif
    /**
     * @brief size
     * @return number of elements
     */
    template<typename Tout = int>
    Tout size() const;

    static DataSpace Null;
    static DataSpace Scalar;
    static DataSpace linear(hsize_t length)
    {
        return DataSpace{Dimensions{length}};
    }
    /// overload for signed integral types
    template <typename Tint, GtH5::traits::if_signed_integral<Tint> = true>
    static DataSpace linear(Tint length)
    {
        return linear(static_cast<hsize_t>(length));
    }

    /**
     * @brief DataSpace
     */
    DataSpace();
    explicit DataSpace(std::initializer_list<hsize_t> initlist);
    explicit DataSpace(Dimensions const& dimensions);
    explicit DataSpace(H5::DataSpace dataspace);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataSpace const& toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const override;

    /**
     * @brief number of dimensions
     * @return nDims
     */
    int nDims() const;

    /**
     * @brief Whether this dataspace is scalar. Will return false if space has
     * one dimension and one element.
     * @return true if scalar.
     */
    bool isScalar() const;

    /**
     * @brief isNull
     * @return true if null
     */
    bool isNull() const;

    /**
     * @brief vector containing the size of each dimensions where the length
     * equals nDims.
     * @return dimensions
     */
    Dimensions dimensions() const;

    /**
     * @brief Selection size of this dataspace. If selection was not explicitly
     * set, selection size is equal to size.
     * @return number of elements currently selected
     */
    hssize_t selectionSize() const;

private:

    /// hdf5 base instance
    H5::DataSpace m_dataspace;
};

/// selection operator
using SelectionOp = H5S_seloper_t;

class GTH5_EXPORT DataSpaceSelection
{
public:

    /**
      * @brief Helper class for selecting a hyperslap of a dataset
      * @param dspace Dataspace representing the layout of either the file or
      * memory dataspace
      * @param count Block/number of elements in each dimension. Defaults to
      * dataspace provided
      * @param offset Offset in each dimension. Defaults to 0
      * @param stride Step size. Defaults to 1 in each dimension
      * @param block Block layout. Defaults to 1 in each dimension
      * @param op Selection operation (i.e. and, or, xor, etc.)
      */
    explicit DataSpaceSelection(DataSpace dspace,
                                Dimensions count = {}, Dimensions offset = {},
                                Dimensions stride = {}, Dimensions block = {},
                                SelectionOp op = H5S_SELECT_SET) :
        m_space{std::move(dspace)},
        m_count{std::move(count)},
        m_offset{std::move(offset)},
        m_stride{std::move(stride)},
        m_block{std::move(block)},
        m_op{op}
    { }

    bool commit();

    hssize_t size() { commit(); return m_space.selectionSize(); }

    DataSpace const& space() { commit(); return m_space; }

    Dimensions const& count() const { return m_count; }
    void setCount(Dimensions count) { m_count = std::move(count); }

    Dimensions const& offset() const { return m_offset; }
    void setOffset(Dimensions offset) { m_offset = std::move(offset); }

    Dimensions const& stride() const { return m_stride; }
    void setStride(Dimensions stride) { m_stride = std::move(stride); }

    Dimensions const& block() const { return m_block; }
    void setBlock(Dimensions block) { m_block = std::move(block); }

    SelectionOp op() const { return m_op; }
    void setOp(SelectionOp op) { m_op = op; }

    operator DataSpace const&() { return space(); }

private:

    DataSpace m_space{};
    Dimensions m_count{};
    Dimensions m_offset{};
    Dimensions m_stride{};
    Dimensions m_block{};

    SelectionOp m_op = H5S_SELECT_SET;

    static bool testSelection(Dimensions& dims,
                              Dimensions const& sDims,
                              hsize_t fillValue);
};

template<typename Tout>
inline Tout
DataSpace::size() const
{
    return isScalar() ? 1 : GtH5::prod(dimensions());
}

/**
  * @brief Helper method for ceating/selecting a hyperslap in a dataspace.
  * @param dspace Dataspace representing the layout of either the file or
  * memory dataspace
  * @param count Block/number of elements in each dimension. Defaults to
  * dataspace provided
  * @param offset Offset in each dimension. Defaults to 0
  * @param stride Step size. Defaults to 1 in each dimension
  * @param block Block layout. Defaults to 1 in each dimension
  * @param op Selection operation (i.e. and, or, xor, etc.)
  * @return Dataspace representing the selection
  */
inline DataSpaceSelection makeSelection(DataSpace const& dspace,
                                        Dimensions count  = {},
                                        Dimensions offset = {},
                                        Dimensions stride = {},
                                        Dimensions block  = {},
                                        SelectionOp op = H5S_SELECT_SET)
{
    return DataSpaceSelection{dspace, count, offset, stride, block, op};
}

} // namespace GtH5

// operators
GTH5_EXPORT bool operator==(GtH5::DataSpace const& first,
                            GtH5::DataSpace const& other);
GTH5_EXPORT bool operator!=(GtH5::DataSpace const& first,
                            GtH5::DataSpace const& other);

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5DataSpace = GtH5::DataSpace;
#endif

GTH5_EXPORT std::ostream& operator<<(std::ostream& s, GtH5::DataSpace const& d);
GTH5_EXPORT QDebug operator<<(QDebug s, GtH5::DataSpace const& d);

#endif // GTH5_DATASPACE_H
