/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5GROUP_H
#define GTH5GROUP_H

#include "gth5_node.h"

class GtH5File;
class GtH5DataType;
class GtH5DataSpace;
class GtH5DataSet;

/**
 * @brief The GtH5Group class
 */
class GTH5_EXPORT GtH5Group : public GtH5Node
{
public:

    static GtH5Group create(const GtH5Group& parent,
                            const QByteArray& name);
    static GtH5Group open(const GtH5Group& parent,
                          const QByteArray& name);

    /**
     * @brief GtH5Group
     */
    GtH5Group();
    explicit GtH5Group(GtH5File& file);
    GtH5Group(std::shared_ptr<GtH5File> file,
              H5::Group const& group,
              QByteArray const& name = {});

    GtH5Group(GtH5Group const& other) = default;
    GtH5Group(GtH5Group&& other) = default;
    GtH5Group& operator=(GtH5Group const& other);
    GtH5Group& operator=(GtH5Group&& other) noexcept;

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::Group toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    int64_t id() const override;

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
     * @brief explicitly closes the resource handle
     */
    void close();

    // groups
    GtH5Group createGroup(QString const& name);
    GtH5Group createGroup(QByteArray const& name);
    GtH5Group openGroup(QString const& name);
    GtH5Group openGroup(QByteArray const& name);

    // datasets
    GtH5DataSet createDataset(QString const& name,
                              GtH5DataType const& dtype,
                              GtH5DataSpace const& dspace);
    GtH5DataSet createDataset(QByteArray const& name,
                              GtH5DataType const& dtype,
                              GtH5DataSpace const& dspace);
    GtH5DataSet openDataset(QString const& name);
    GtH5DataSet openDataset(QByteArray const& name);

    void swap(GtH5Group& other) noexcept;

private:

    /// hdf5 base instance
    H5::Group m_group{};

    friend class GtH5Reference;
};

GTH5_EXPORT void swap(GtH5Group& first, GtH5Group& other) noexcept;

#endif // GTH5GROUP_H
