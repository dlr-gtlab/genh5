/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5GROUP_H
#define GTH5GROUP_H

#include "gt_h5node.h"

class GtH5File;

/**
 * @brief The GtH5Group class
 */
class GT_H5_EXPORT GtH5Group : public GtH5Node
{
public:

    /**
     * @brief The AccessFlag enum
     */
    enum AccessFlag
    {
        Create,         // fails if group does exist
        CreateOpen,     // creates group if it does not exist
    };

    static GtH5Group create(const GtH5Node& parent,
                            const QByteArray& name,
                            AccessFlag flag);
    static GtH5Group open(const GtH5Node& parent,
                          const QByteArray& name);

    /**
     * @brief GtH5Group
     */
    GtH5Group() {}
    GtH5Group(GtH5File& file);
    GtH5Group(GtH5File& file,
              const H5::Group& group,
              const QByteArray& name = QByteArray());

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

private:

    /// hdf5 base instance
    H5::Group m_group;

    friend class GtH5Reference;
};

#endif // GTH5GROUP_H
