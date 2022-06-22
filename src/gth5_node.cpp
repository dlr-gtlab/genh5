/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_node.h"

#include "gth5_attribute.h"
#include "gth5_dataspace.h"
#include "gth5_datatype.h"


GtH5::Node::Node(std::shared_ptr<File> file) :
    Location{std::move(file)}
{

}

H5::H5Location const*
GtH5::Node::toH5Location() const
{
    return toH5Object();
}

bool
GtH5::Node::hasAttribute(String const& name) const
{
    return toH5Object()->attrExists(name.constData());
}

GtH5::Attribute
GtH5::Node::createAttribute(QString const& name,
                            DataType const& dtype,
                            DataSpace const& dspace) const
{
    return createAttribute(name.toUtf8(), dtype, dspace);
}

GtH5::Attribute
GtH5::Node::createAttribute(String const& name,
                            DataType const& dtype,
                            DataSpace const& dspace) const
{
    auto const& parent = *this;

    // create new attribute!
    if (!parent.hasAttribute(name))
    {
        H5::Attribute attr;
        try
        {
            attr = parent.toH5Object()->createAttribute(name.constData(),
                                                        dtype.toH5(),
                                                        dspace.toH5());
        }
        catch (H5::AttributeIException& /*e*/)
        {
            qCritical() << "HDF5: Creating attribute failed! -" << name;
            return {};
        }
        catch (H5::Exception& /*e*/)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5::Attribute::create failed! -"
                        << name;
            return {};
        }

        return {parent.file(), std::move(attr)};
    }

    // open existing attribute
    auto attr = openAttribute(name);

    // check if memory layout is equal
    if (attr.dataType() == dtype && attr.dataSpace() == dspace)
    {
        return attr;
    }

    // attribute cannot be resized and must be deleted
    qWarning() << "HDF5: Invalid memory layout! Overwriting dataset! -" << name;
    if (!attr.deleteLink())
    {
        return {};
    }

    return createAttribute(std::move(name), dtype, dspace);
//    return GtH5Attribute::create(*this, name, dtype, dspace);
}

GtH5::Attribute
GtH5::Node::openAttribute(QString const& name) const
{
    return openAttribute(name.toUtf8());
}

GtH5::Attribute
GtH5::Node::openAttribute(String const& name) const
{
    auto const& parent = *this;
    H5::Attribute attr;
    try
    {
        attr = parent.toH5Object()->openAttribute(name.constData());
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Opening attribute failed! -" << name;
        return {};
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Attribute::open failed! -"
                    << name;
        return {};
    }

    return {parent.file(), std::move(attr)};
}
