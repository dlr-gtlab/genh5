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
    explicit Attribute(std::shared_ptr<File> file, H5::Attribute attr);

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    [[deprecated("use id() instead")]]
    H5::Attribute const& toH5() const noexcept;
#endif

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

protected:

    /// Returns the abstract hdf5 dataset
    H5::AbstractDs const& toH5AbsDataSet() const noexcept override;

    /// write implementation
    bool doWrite(void const* data, DataType const&) const override;
    /// read implementation
    bool doRead(void* data, DataType const&) const override;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    H5::H5Location const* toH5Location() const noexcept override;

private:

    /// hdf5 base instance
    H5::Attribute m_attribute{};

    friend class Reference;
};

/**
 * @brief helper function to retrieve the name of an attribute
 * @param attr object
 * @return name
 */
GENH5_EXPORT String getAttributeName(Attribute const& attr) noexcept;

} // namespace GenH5

#endif // GENH5_ATTRIBUTE_H
