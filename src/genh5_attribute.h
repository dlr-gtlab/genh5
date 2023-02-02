/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_ATTRIBUTE_H
#define GENH5_ATTRIBUTE_H

#include "genh5_location.h"
#include "genh5_abstractdataset.h"

namespace GenH5
{

/**
 * @brief The Attribute class
 */
class GENH5_EXPORT Attribute : public Location,
                               public AbstractDataSet
{
public:

    /**
     * @brief Attribute
     */
    Attribute();
    explicit Attribute(hid_t id);

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const noexcept override;

    /**
     * @brief deletes the object.
     */
    void deleteLink() noexcept(false) override;

    /**
     * @brief attribute name
     * @return name
     */
    String name() const noexcept override;

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

    /**
     * @brief dataType of this dataset
     * @return dataType
     */
    DataType dataType() const noexcept(false) override;
    /**
     * @brief dataSpace of this dataset
     * @return dataSpace
     */
    DataSpace dataSpace() const noexcept(false) override;

    /// swaps all members
    void swap(Attribute& other) noexcept;

protected:

    /// write implementation
    bool doWrite(void const* data, DataType const&) const override;
    /// read implementation
    bool doRead(void* data, DataType const&) const override;

private:

    /// attribute id
    IdComponent<H5I_ATTR> m_id;

    friend class Reference;
};

/**
 * @brief helper function to retrieve the name of an attribute
 * @param attr object
 * @return name
 */
GENH5_EXPORT String getAttributeName(Attribute const& attr) noexcept;

} // namespace GenH5

inline void
swap(GenH5::Attribute& a, GenH5::Attribute& b) noexcept
{
    a.swap(b);
}

#endif // GENH5_ATTRIBUTE_H
