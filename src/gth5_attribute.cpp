/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_attribute.h"
#include "gth5_node.h"
#include "gth5_file.h"
#include "gth5_exception.h"

#include <QDebug>

GtH5::String
GtH5::getAttributeName(Attribute const& attr) noexcept
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

GtH5::Attribute::Attribute() = default;

GtH5::Attribute::Attribute(std::shared_ptr<File> file, H5::Attribute attr) :
    Location{std::move(file)},
    m_attribute(std::move(attr))
{ }

hid_t
GtH5::Attribute::id() const noexcept
{
    return m_attribute.getId();
}

H5::H5Location const*
GtH5::Attribute::toH5Location() const noexcept
{
    return &m_attribute;
}

H5::Attribute const&
GtH5::Attribute::toH5() const noexcept
{
    return m_attribute;
}

bool
GtH5::Attribute::doWrite(void const* data, DataType const& dtype) const
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
GtH5::Attribute::doRead(void* data, DataType const& dtype) const
{
    m_attribute.read(dtype.toH5(), data);
    return true;
}

void
GtH5::Attribute::deleteLink() noexcept(false)
{
    qDebug() << "HDF5: Deleting attribute...";

    if (!isValid())
    {
        throw LocationException{"Deleting attribute failed "
                                "(Invalid attribute)"};
    }

    // returns error type
    if (H5Adelete_by_name(file()->id(), path().constData(),
                          name().constData(), H5P_DEFAULT))
    {
        throw LocationException{"Deleting attribute failed"};
    }

    close();
}

GtH5::String
GtH5::Attribute::name() const noexcept
{
    return getAttributeName(*this);
}

void
GtH5::Attribute::close()
{
    m_attribute.close();
}

H5::AbstractDs const&
GtH5::Attribute::toH5AbsDataSet() const noexcept
{
    return m_attribute;
}
