/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATASPACE_H
#define GTH5DATASPACE_H

#include "gt_h5object.h"
#include <QVector>

/**
 * @brief The GtH5DataSpace class
 */
class GT_H5_EXPORT GtH5DataSpace : public GtH5Object
{
public:

    template<typename Tin, typename Tout = int>
    static Tout sum(const QVector<Tin>& dimensions);
    template<typename Tout = int>
    Tout sum() const;

    /**
     * @brief GtH5DataSpace
     */
    GtH5DataSpace();
    explicit GtH5DataSpace(uint64_t simpleLength);
    explicit GtH5DataSpace(QVector<uint64_t> const& dimensions);
    explicit GtH5DataSpace(H5::DataSpace const& dataspace);

    GtH5DataSpace(GtH5DataSpace const& other);
    GtH5DataSpace(GtH5DataSpace&& other) noexcept;
    GtH5DataSpace& operator=(GtH5DataSpace const& other);
    GtH5DataSpace& operator=(GtH5DataSpace&& other) noexcept;

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataSpace toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    int64_t id() const override;

    /**
     * @brief number of dimensions
     * @return nDims
     */
    int nDims() const;

    /**
     * @brief vector containing the size of each dimensions where the length
     * equals nDims.
     * @return dimensions
     */
    QVector<uint64_t> dimensions() const;

   void swap(GtH5DataSpace& other) noexcept;

private:

    /// hdf5 base instance
    H5::DataSpace m_dataspace;
};

// operators
GT_H5_EXPORT bool operator==(GtH5DataSpace const& first,
                             GtH5DataSpace const& other);
GT_H5_EXPORT bool operator!=(GtH5DataSpace const& first,
                             GtH5DataSpace const& other);

GT_H5_EXPORT void swap(GtH5DataSpace& first, GtH5DataSpace& other) noexcept;

template<typename Tin, typename Tout> Tout inline
GtH5DataSpace::sum(const QVector<Tin>& dimensions)
{
    return std::accumulate(std::begin(dimensions), std::end(dimensions),
                    Tout{!dimensions.isEmpty()}, [](Tout sum, Tin dim){
        return sum * dim;
    });
}

template<typename Tout> Tout inline
GtH5DataSpace::sum() const
{
    return sum<uint64_t, Tout>(dimensions());
}


#endif // GTH5DATASPACE_H
