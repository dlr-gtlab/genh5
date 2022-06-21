/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_GROUP_H
#define GTH5_GROUP_H

#include "gth5_node.h"

#include "gth5_datasetcproperties.h"
#include "gth5_utils.h"
#include "gth5_optional.h"

namespace GtH5
{
// forward decl
class DataType;
class File;
class DataSpace;
class DataSet;

/**
 * @brief The Group class
 */
class GTH5_EXPORT Group : public Node
{
public:

    /**
     * @brief Group
     */
    Group();
    explicit Group(File const& file);
    Group(std::shared_ptr<File> file,
          H5::Group group,
          String name = {});

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::Group const& toH5() const;

    /**
     * @brief id or handle of the hdf5 resource
     * @return id
     */
    hid_t id() const override;

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
     * @brief explicitly closes the resource handle
     */
    void close();

    // groups
    Group createGroup(QString const& name);
    Group createGroup(String name);
    Group openGroup(QString const& name);
    Group openGroup(String name);

    // datasets
    GtH5::DataSet createDataset(QString const& name,
                                DataType const& dtype,
                                DataSpace const& dspace,
                                Optional<DataSetCProperties> properties = {});
    GtH5::DataSet createDataset(String name,
                                DataType const& dtype,
                                DataSpace const& dspace,
                                Optional<DataSetCProperties> properties = {});
    GtH5::DataSet openDataset(QString const& name);
    GtH5::DataSet openDataset(String name);

protected:

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    H5::H5Object const* toH5Object() const override;

private:

    /// hdf5 base instance
    H5::Group m_group{};

    friend class Reference;
};

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
using GtH5Group = GtH5::Group;
#endif

#endif // GTH5_GROUP_H
