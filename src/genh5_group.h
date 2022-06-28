/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_GROUP_H
#define GENH5_GROUP_H

#include "genh5_node.h"

#include "genh5_datasetcproperties.h"
#include "genh5_utils.h"
#include "genh5_optional.h"

namespace GenH5
{
// forward decl
class DataType;
class File;
class DataSpace;
class DataSet;

/**
 * @brief The Group class
 */
class GENH5_EXPORT Group : public Node
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
    Group createGroup(String const& name)noexcept(false);
    Group openGroup(String const& name) noexcept(false);

    // datasets
    DataSet createDataset(String const& name,
                  DataType const& dtype, DataSpace const& dspace,
                  Optional<DataSetCProperties> cProps = {}) noexcept(false);
    DataSet openDataset(String const& name) noexcept(false);

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

} // namespace GenH5

#endif // GENH5_GROUP_H
