/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5ATTRIBUTE_H
#define GTH5ATTRIBUTE_H

#include "gt_h5location.h"
#include "gt_h5abtsractdataset.h"

class GtH5Node;

/**
 * @brief The GtH5Attribute class
 */
class GT_H5_EXPORT GtH5Attribute : public GtH5Location,
                                   public GtH5AbtsractDataSet
{
public:

    static GtH5Attribute create(GtH5Node const& parent,
                                QByteArray const& name,
                                GtH5DataType const& dtype,
                                GtH5DataSpace const& dspace);

    static GtH5Attribute open(GtH5Node const& parent,
                              QByteArray const& name);

    /**
     * @brief GtH5Attribute
     */
    GtH5Attribute();
    GtH5Attribute(std::shared_ptr<GtH5File> file,
                  H5::Attribute const& attr,
                  QByteArray const& name = {});

    GtH5Attribute(GtH5Attribute const& other);
    GtH5Attribute(GtH5Attribute&& other) noexcept;
    GtH5Attribute& operator=(GtH5Attribute const& other);
    GtH5Attribute& operator=(GtH5Attribute&& other)  noexcept;

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::Attribute toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    int64_t id() const override;

    /**
     * @brief returns whether the object id is valid and can be used for further
     * actions. Call this after instantion to verify the resource allocation.
     * @return whether object id is valid
     */
    bool isValid() const override;

    /**
     * @brief deletes the object.
     * @return success
     */
    bool deleteLink() override;    

    /**
     * @brief type of the object
     * @return type
     */
    ObjectType type() const override;

    /**
     * @brief returns the hdf5 object as a h5location.
     * @return h5location
     */
    H5::H5Location const* toH5Location() const override;

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

    void swap(GtH5Attribute& other) noexcept;

protected:

    bool doWrite(void const* data) const override;
    bool doRead(void* data) const override;

private:

    /// hdf5 base instance
    H5::Attribute m_attribute{};

    friend class GtH5Reference;
};

GT_H5_EXPORT void swap(GtH5Attribute& first, GtH5Attribute& other) noexcept;

#endif // GTH5ATTRIBUTE_H
