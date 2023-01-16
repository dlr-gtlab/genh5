/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_attribute.h"
#include "genh5_node.h"
#include "genh5_file.h"
#include "genh5_exception.h"

#include <QDebug>

GenH5::String
GenH5::getAttributeName(Attribute const& attr) noexcept
{
    if (!attr.isValid())
    {
        return {};
    }

    String buffer{32, ' '};
    size_t bufferLen = static_cast<size_t>(buffer.size());

    auto acutalLen = static_cast<size_t>(H5Aget_name(attr.id(),
                                                     bufferLen,
                                                     buffer.data()));

    if (acutalLen > bufferLen)
    {
        bufferLen = acutalLen + 1;
        buffer.resize(static_cast<int>(bufferLen));
        H5Aget_name(attr.id(), bufferLen, buffer.data());
    }

    // remove of excess whitespaces and trailing '\0'
    return buffer.trimmed().chopped(1);
}

GenH5::Attribute::Attribute() = default;

GenH5::Attribute::Attribute(std::shared_ptr<File> file, H5::Attribute attr) :
    Location{std::move(file)},
    m_attribute(std::move(attr))
{ }

hid_t
GenH5::Attribute::id() const noexcept
{
    return m_attribute.getId();
}

H5::H5Location const*
GenH5::Attribute::toH5Location() const noexcept
{
    return &m_attribute;
}

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
H5::Attribute const&
GenH5::Attribute::toH5() const noexcept
{
    return m_attribute;
}
#endif

bool
GenH5::Attribute::doWrite(void const* data, DataType const& dtype) const
{
    try
    {
        m_attribute.write(dtype.toH5(), data);
        return true;
    }
    catch (H5::AttributeIException const& e)
    {
        qCritical() << "HDF5: Writing attribute failed! -" << name();
        throw AttributeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Writing attribute failed! -"
                    << name();
        throw AttributeException{e.getCDetailMsg()};
    }
}

bool
GenH5::Attribute::doRead(void* data, DataType const& dtype) const
{
    try
    {
        m_attribute.read(dtype.toH5(), data);
        return true;
    }
    catch (H5::AttributeIException const& e)
    {
        qCritical() << "HDF5: Reading attribute failed! -" << name();
        throw AttributeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Reading attribute failed! -"
                    << name();
        throw AttributeException{e.getCDetailMsg()};
    }
}

void
GenH5::Attribute::deleteLink() noexcept(false)
{
    qDebug() << "HDF5: Deleting attribute...";

    if (!isValid())
    {
        throw LocationException{"Deleting attribute failed "
                                "(Invalid attribute)"};
    }

    // returns error type
    if (H5Adelete_by_name(file()->id(), path().constData(),
                          name().constData(), H5P_DEFAULT) < 0)
    {
        throw LocationException{"Deleting attribute failed"};
    }

    close();
}

GenH5::String
GenH5::Attribute::name() const noexcept
{
    return getAttributeName(*this);
}

void
GenH5::Attribute::close()
{
    m_attribute.close();
}

H5::AbstractDs const&
GenH5::Attribute::toH5AbsDataSet() const noexcept
{
    return m_attribute;
}
