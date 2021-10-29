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

class GtH5Leaf;

/**
 * @brief The GtH5Attribute class
 */
class GT_H5_EXPORT GtH5Attribute : public GtH5Location,
                                   public GtH5AbtsractDataSet
{
public:

    static GtH5Attribute create(const GtH5Leaf& parent,
                                const QByteArray& name,
                                const GtH5DataType& dtype,
                                const GtH5DataSpace& dspace,
                                AccessFlag flag);

    static GtH5Attribute open(const GtH5Leaf& parent,
                              const QByteArray& name);

    /**
     * @brief GtH5Attribute
     */
    GtH5Attribute() {}
    GtH5Attribute(GtH5File& file,
                  const H5::Attribute& attr,
                  const QByteArray& name = QByteArray());

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
    const H5::H5Location* toH5Location() const override;

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

protected:

    bool doWrite(void* data) const override;
    bool doRead(void* data) const override;

private:

    /// hdf5 base instance
    H5::Attribute m_attribute;

    friend class GtH5Reference;
};

#endif // GTH5ATTRIBUTE_H
