/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5DATASET_H
#define GTH5DATASET_H

#include "gth5_node.h"
#include "gth5_abtsractdataset.h"
#include "gth5_reference.h"
#include "gth5_datasetproperties.h"


class GtH5Node;
class GtH5Reference;

/**
 * @brief The GtH5DataSet class
 */
class GTH5_EXPORT GtH5DataSet : public GtH5Node,
                                 public GtH5AbtsractDataSet
{
public:

    static GtH5DataSet create(GtH5Group const& parent,
                              QByteArray const& name,
                              GtH5DataType const& dtype,
                              GtH5DataSpace const& dspace,
                              GtH5DataSetProperties const& properties = {});

    static GtH5DataSet open(GtH5Group const& parent,
                            QByteArray const& name);

    /**
     * @brief GtH5DataSet
     */
    GtH5DataSet();
    GtH5DataSet(std::shared_ptr<GtH5File> file,
                H5::DataSet const & dset,
                QByteArray const& name = {});

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
    H5::H5Object const* toH5Object() const override;

    /**
     * @brief properties used to create this object.
     * @return create properties
     */
    GtH5DataSetProperties const& properties() const;

    /**
     * @brief resizes this dataset.
     * @param dimensions new dimensions. Cannot exceed the original dimensions.
     * Number of dimensions must match the current number
     * @return success
     */
    bool resize(const QVector<uint64_t>& dimensions);

    /**
     * @brief explicitly closes the resource handle
     */
    void close();

protected:

    bool doWrite(void const* data) const override;
    bool doRead(void* data) const override;

private:

    /// hdf5 base instance
    H5::DataSet m_dataset{};
    /// dataset create properties associated with this object
    GtH5DataSetProperties m_properties{};

    friend class GtH5Reference;
};

#endif // GTH5DATASET_H
