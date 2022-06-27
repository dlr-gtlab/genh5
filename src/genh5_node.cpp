/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_node.h"

#include "genh5_attribute.h"
#include "genh5_dataspace.h"
#include "genh5_datatype.h"
#include "genh5_version.h"

#include <QDebug>

GenH5::Node::Node(std::shared_ptr<File> file) noexcept :
    Location{std::move(file)}
{ }

H5::H5Location const*
GenH5::Node::toH5Location() const noexcept
{
    return toH5Object();
}

bool
GenH5::Node::hasAttribute(String const& name) const
{
    try
    {
        return isValid() && toH5Object()->attrExists(name.constData());
    }
    catch (H5::Exception const& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] Node::hasAttribute";
        return false;
    }
}

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
GenH5::Attribute
GenH5::Node::createAttribute(QString const& name,
                            DataType const& dtype,
                            DataSpace const& dspace) const noexcept(false)
{
    return createAttribute(name.toUtf8(), dtype, dspace);
}

GenH5::Attribute
GenH5::Node::openAttribute(QString const& name) const noexcept(false)
{
    return openAttribute(name.toUtf8());
}
#endif

GenH5::Attribute
GenH5::Node::createAttribute(String const& name,
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

GenH5::Attribute
GenH5::Node::openAttribute(String const& name) const noexcept(false)
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

GenH5::String
GenH5::Node::versionAttrName()
{
    return QByteArrayLiteral("GENH5_VERSION");
}

bool
GenH5::Node::hasVersionAttribute() const
{
    return hasAttribute(versionAttrName());
}

bool
GenH5::Node::createVersionAttribute() const noexcept(false)
{
    auto version = Version::current();
    auto attr = createAttribute(versionAttrName(),
                                DataType::Version,
                                DataSpace::Scalar);
    return attr.write(&version);
}

GenH5::Version
GenH5::Node::readVersionAttribute() const noexcept(false)
{
    Version version{};
    auto attr = openAttribute(versionAttrName());
    if (attr.dataType() != DataType::Version ||
        attr.dataSpace() != DataSpace::Scalar)
    {
        throw AttributeException{"Invalid version Attribute format!"};
    }
    attr.read(&version);
    return version;
}
