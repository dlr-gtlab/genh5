/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATASPACE_H
#define GENH5_DATASPACE_H

#include "genh5_idcomponent.h"
#include "genh5_object.h"
#include "genh5_utils.h"

#include "H5Spublic.h"

namespace GenH5
{

/**
 * @brief The DataSpace class
 */
class GENH5_EXPORT DataSpace : public Object
{
public:

    /**
     * @brief size
     * @return number of elements
     */
    template<typename Tout = hsize_t>
    Tout size() const;

    static DataSpace const& Null;
    static DataSpace const& Scalar;

    static DataSpace linear(hsize_t length) noexcept(false)
    {
        return DataSpace{Dimensions{length}};
    }
    /// overload for signed integral types
    template <typename Tint, GenH5::traits::if_signed_integral<Tint> = true>
    static DataSpace linear(Tint length) noexcept(false)
    {
        return linear(static_cast<hsize_t>(length));
    }

    /// Instantiates a new Datatype and assigns the id without incrementing it
    static DataSpace fromId(hid_t id);

    /**
     * @brief DataSpace
     */
    DataSpace();
    explicit DataSpace(H5S_class_t type);
    explicit DataSpace(hid_t id);
    explicit DataSpace(std::initializer_list<hsize_t> initlist) noexcept(false);
    explicit DataSpace(Dimensions const& dimensions) noexcept(false);

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief number of dimensions
     * @return nDims
     */
    int nDims() const noexcept;

    /**
     * @brief Whether this dataspace is scalar. Will return false if space has
     * one dimension and one element.
     * @return true if scalar.
     */
    bool isScalar() const noexcept;

    /**
     * @brief isNull
     * @return true if null
     */
    bool isNull() const noexcept;

    /**
     * @brief vector containing the size of each dimensions where the length
     * equals nDims.
     * @return dimensions
     */
    Dimensions dimensions() const noexcept;

    /**
     * @brief Selection size of this dataspace. If selection was not explicitly
     * set, selection size is equal to size.
     * @return number of elements currently selected
     */
    hssize_t selectionSize() const noexcept;

    /// swaps all members
    void swap(DataSpace& other) noexcept;

private:

    /// dataspace id
    IdComponent<H5I_DATASPACE> m_id;
};

/// selection operator
using SelectionOp = H5S_seloper_t;

class DataSpaceSelection
{
public:

    /**
      * @brief Helper class for selecting a hyperslap of a dataset
      * @param dspace Dataspace representing the layout of either the file or
      * memory dataspace
      * @param count Block/number of elements in each dimension. Defaults to
      * dataspace provided or 1 for each missing dimension.
      * @param offset Offset in each dimension. Defaults to 0.
      * @param stride Step size. Defaults to 1 in each dimension.
      * @param block Block layout. Defaults to 1 in each dimension.
      * @param op Selection operation (i.e. and, or, xor, etc.).
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

    hssize_t size() noexcept(false)
    {
        commit();
        return m_space.selectionSize();
    }

    DataSpace const& space() noexcept(false)
    {
        commit();
        return m_space;
    }

    Dimensions const& count() const noexcept { return m_count; }
    DataSpaceSelection& setCount(Dimensions count) noexcept {
        m_count = std::move(count);
        return *this;
    }

    Dimensions const& offset() const noexcept { return m_offset; }
    DataSpaceSelection& setOffset(Dimensions offset) noexcept {
        m_offset = std::move(offset);
        return *this;
    }

    Dimensions const& stride() const noexcept{ return m_stride; }
    DataSpaceSelection& setStride(Dimensions stride) noexcept {
        m_stride = std::move(stride);
        return *this;
    }

    Dimensions const& block() const noexcept { return m_block; }
    DataSpaceSelection& setBlock(Dimensions block) noexcept {
        m_block = std::move(block);
        return *this;
    }

    SelectionOp op() const noexcept { return m_op; }
    DataSpaceSelection& setOp(SelectionOp op) noexcept {
        m_op = op;
        return *this;
    }

    operator DataSpace const&() noexcept(false) { return space(); }

private:

    DataSpace m_space{};
    Dimensions m_count{};
    Dimensions m_offset{};
    Dimensions m_stride{};
    Dimensions m_block{};
    SelectionOp m_op = H5S_SELECT_SET;

    /**
     * @brief Applies the selection to the dataspace. Must be called before
     * accessing dataspace
     * @throws DataSapceExcetpion
     */
    GENH5_EXPORT void commit() noexcept(false);

    /**
     * @brief Extends input dimension to dataspace dimension and fills missing
     * entries with fillValue.
     * @throws DataSpaceException if dim exceed dataspace dim
     * @param dim dim to test
     * @param sDim dataspace dim
     * @param fillValue value to fill extend dim with
     */
    static void testSelection(Dimensions& dim,
                              Dimensions const& sDim,
                              hsize_t fillValue) noexcept(false);
};

template<typename Tout>
inline Tout
DataSpace::size() const
{
    return isScalar() ? 1 : GenH5::prod<Tout>(dimensions());
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

} // namespace GenH5

// operators
GENH5_EXPORT bool operator==(GenH5::DataSpace const& first,
                            GenH5::DataSpace const& other);
GENH5_EXPORT bool operator!=(GenH5::DataSpace const& first,
                            GenH5::DataSpace const& other);
inline void
swap(GenH5::DataSpace& a, GenH5::DataSpace& b) noexcept
{
    a.swap(b);
}

#endif // GENH5_DATASPACE_H
