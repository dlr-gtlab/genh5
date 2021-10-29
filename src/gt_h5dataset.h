/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATASET_H
#define GTH5DATASET_H

#include "gt_h5leaf.h"
#include "gt_h5abtsractdataset.h"
#include "gt_h5reference.h"
#include "gt_h5datasetproperties.h"


class GtH5Node;
class GtH5Reference;

/**
 * @brief The GtH5DataSet class
 */
class GT_H5_EXPORT GtH5DataSet : public GtH5Leaf,
                                 public GtH5AbtsractDataSet
{
public:

    static GtH5DataSet create(const GtH5Node& parent,
                              const QByteArray& name,
                              const GtH5DataType& dtype,
                              const GtH5DataSpace& dspace,
                              AccessFlag flag);
    static GtH5DataSet create(const GtH5Node& parent,
                              const QByteArray& name,
                              const GtH5DataType& dtype,
                              const GtH5DataSpace& dspace,
                              const GtH5DataSetProperties& properties,
                              AccessFlag flag);
    static GtH5DataSet open(const GtH5Node& parent,
                            const QByteArray& name);

    /**
     * @brief GtH5DataSet
     */
    GtH5DataSet() {}
    GtH5DataSet(GtH5File& file,
                const H5::DataSet& dset,
                const QByteArray& name = QByteArray());

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::DataSet toH5() const;

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
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    const H5::H5Object* toH5Object() const override;

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    GtH5DataSetProperties properties() const;

    /**
     * @brief resizes this dataset.
     * @param new dimensions. Cannot exceed the original dimensions. Number of
     * dimensions must match the current number
     * @return success
     */
    bool resize(const QVector<uint64_t>& dimensions);

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

protected:

    bool doWrite(void* data) const override;
    bool doRead(void* data) const override;

private:

    /// hdf5 base instance
    H5::DataSet m_dataset;
    /// dataset create properties associated with this object
    GtH5DataSetProperties m_properties;

    friend class GtH5Reference;
};

#endif // GTH5DATASET_H
