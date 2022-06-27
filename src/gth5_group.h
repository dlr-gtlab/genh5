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
    Group(std::shared_ptr<File> file, H5::Group group);

    /**
     * @brief allows access of the base hdf5 object
     * @return base hdf5 object
     */
    H5::Group const& toH5() const noexcept;

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
     * @brief explicitly closes the resource handle
     */
    void close();

    // groups
    Group createGroup(QString const& name) noexcept(false);
    Group createGroup(String const& name)noexcept(false);
    Group openGroup(QString const& name) noexcept(false);
    Group openGroup(String const& name) noexcept(false);

    // datasets
    GtH5::DataSet createDataset(QString const& name,
                                DataType const& dtype,
                                DataSpace const& dspace,
                                Optional<DataSetCProperties> properties = {}
                                ) noexcept(false);
    GtH5::DataSet createDataset(String const& name,
                                DataType const& dtype,
                                DataSpace const& dspace,
                                Optional<DataSetCProperties> properties = {}
                                ) noexcept(false);
    GtH5::DataSet openDataset(QString const& name) noexcept(false);
    GtH5::DataSet openDataset(String const& name) noexcept(false);

protected:

    /**
     * @brief returns the hdf5 object as a h5object
     * @return h5object
     */
    H5::H5Object const* toH5Object() const noexcept override;

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
