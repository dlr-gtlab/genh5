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


GtH5::Node::Node(std::shared_ptr<File> file) noexcept :
    Location{std::move(file)}
{ }

H5::H5Location const*
GtH5::Node::toH5Location() const noexcept
{
    return toH5Object();
}

bool
GtH5::Node::hasAttribute(String const& name) const noexcept(false)
{
    return toH5Object()->attrExists(name.constData());
}

GtH5::Attribute
GtH5::Node::createAttribute(QString const& name,
                            DataType const& dtype,
                            DataSpace const& dspace) const noexcept(false)
{
    return createAttribute(name.toUtf8(), dtype, dspace);
}

GtH5::Attribute
GtH5::Node::createAttribute(String const& name,
                            DataType const& dtype,
                            DataSpace const& dspace) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw AttributeException{"Creating attribute failed (invalid parent)"};
    }

    // create new attribute!
    if (!parent.hasAttribute(name))
    {
        try
        {
            auto attr = parent.toH5Object()->createAttribute(
                        name.constData(), dtype.toH5(), dspace.toH5());
            return {parent.file(), std::move(attr)};
        }
        catch (H5::AttributeIException const& e)
        {
            throw AttributeException{e.getCDetailMsg()};
        }
        catch (H5::Exception const& e)
        {
            qCritical() << "HDF5: [EXCEPTION] Node::createAttribute";
            throw AttributeException{e.getCDetailMsg()};
        }
    }

    // open existing attribute
    auto attr = openAttribute(name);

    // check if memory layout is equal
    if (attr.dataType() == dtype && attr.dataSpace() == dspace)
    {
        return attr;
    }

    // attribute cannot be resized and must be deleted
    qWarning() << "HDF5: Invalid memory layout! Overwriting attribute! -"
               << name;
    attr.deleteLink();

    return createAttribute(std::move(name), dtype, dspace);
}

GtH5::Attribute
GtH5::Node::openAttribute(QString const& name) const noexcept(false)
{
    return openAttribute(name.toUtf8());
}

GtH5::Attribute
GtH5::Node::openAttribute(String const& name) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw AttributeException{"Opening attribute failed (invalid parent)"};
    }

    try
    {
        auto attr = parent.toH5Object()->openAttribute(name.constData());
        return {parent.file(), std::move(attr)};
    }
    catch (H5::AttributeIException const& e)
    {
        throw AttributeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Node::openAttribute";
        throw AttributeException{e.getCDetailMsg()};
    }
}
