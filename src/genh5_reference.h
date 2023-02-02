/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_REFERENCE_H
#define GENH5_REFERENCE_H

#include "genh5_location.h"

#include "H5Rpublic.h"

namespace GenH5
{
// forward decl
class File;
class Group;
class DataSet;
class Attribute;

/**
 * @brief The Reference class
 */
class GENH5_EXPORT Reference
{
public:

    static constexpr auto bufferSize = H5R_REF_BUF_SIZE;

    using Alignment  = decltype (H5R_ref_t::u.align);
    using BufferType = decltype (*H5R_ref_t::u.__data);

    Reference();
    explicit Reference(H5R_ref_t ref)  noexcept;
    explicit Reference(Alignment data)  noexcept;
    explicit Reference(QByteArray buffer) noexcept(false);
    explicit Reference(Location const& location) noexcept(false);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5R_ref_t const& toH5() const noexcept;

    /**
     * @brief returns whether the reference is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether reference is valid
     */
    bool isValid() const noexcept;

    /**
     * @brief returns the alignment of the reference. May be used to serialize
     * a reference. Only works for datasets and groups.
     * @return alignment of reference
     */
    Alignment alignment() const noexcept
    {
        return m_ref.u.align;
    }

    /**
     * @brief returns the reference data buffer as a QByteArray. Can be used to
     * serialize and deserialize a reference.
     * @return data array
     */
    QByteArray buffer() const noexcept;

    /**
     * @brief try dereferencing the object to a group.
     * @param file in which the object is located
     * @return group
     */
    Group toGroup(File const& file) const noexcept(false);

    /**
     * @brief try dereferencing the object to a dataset.
     * @param file in which the object is located
     * @return dataset
     */
    DataSet toDataSet(File const& file) const noexcept(false);

    /**
     * @brief try dereferencing the object to a attribute.
     * @param file in which the object is located
     * @return attribute
     */
    Attribute toAttribute(File const& file) const noexcept(false);

private:

    /// hdf5 base instance
    H5R_ref_t m_ref{};
};

} // namespace GenH5

#endif // GENH5_REFERENCE_H
