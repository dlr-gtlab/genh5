/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_REFERENCE_H
#define GTH5_REFERENCE_H

#include "gth5_location.h"


namespace GtH5
{
// forward decl
class File;
class Group;
class DataSet;
class Attribute;

/**
 * @brief The Reference class
 */
class GTH5_EXPORT Reference
{
public:

    static constexpr auto bufferSize = H5R_REF_BUF_SIZE;

    using Alignment  = decltype (H5R_ref_t::u.align);
    using BufferType = decltype (*H5R_ref_t::u.__data);

    Reference();
    explicit Reference(Alignment data,
                       ObjectType type = ObjectType::UnkownType);
    explicit Reference(QByteArray buffer,
                       ObjectType type = ObjectType::UnkownType);
    explicit Reference(H5R_ref_t ref,
                       ObjectType type = ObjectType::UnkownType);
    explicit Reference(Location const& location);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5R_ref_t const& toH5() const;

    /**
     * @brief returns whether the reference is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether reference is valid
     */
    bool isValid() const;


#ifndef GTH5_NO_DEPRECATED_SYMBOLS
    /**
     * @brief returns the alignment of the reference. May be used to serialize
     * a reference. Only works for datasets and groups. Prefer buffer() instead.
     * @return alignment of reference
     */
    Alignment data() const { return alignment(); }
#endif

    /**
     * @brief returns the alignment of the reference. May be used to serialize
     * a reference. Only works for datasets and groups. Prefer buffer() instead.
     * @return alignment of reference
     */
    Alignment alignment() const;

    /**
     * @brief returns the reference data buffer as a QByteArray. Can be used to
     * serialize and deserialize a reference.Prefer iver data().
     * @return data array
     */
    QByteArray buffer() const;

    /**
     * @brief type of the referenced object (may be Unkown)
     * @return type
     */
    ObjectType type() const;

    /**
     * @brief try dereferencing the object to a group.
     * @param file in which the object is located
     * @return group
     */
    Group toGroup(File const& file) const;

    /**
     * @brief try dereferencing the object to a dataset.
     * @param file in which the object is located
     * @return dataset
     */
    DataSet toDataSet(File const& file) const;

    /**
     * @brief try dereferencing the object to a attribute.
     * @param file in which the object is located
     * @return attribute
     */
    Attribute toAttribute(File const& file) const;

private:

    /// hdf5 base instance
    H5R_ref_t m_ref{};
    /// type of the referenced object
    ObjectType m_type{ObjectType::UnkownType};
};

inline Reference::Alignment
Reference::alignment() const
{
    return m_ref.u.align;
}

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5Reference = GtH5::Reference;
#endif

#endif // GTH5_REFERENCE_H
