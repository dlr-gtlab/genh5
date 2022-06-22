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

#include <QDebug>

GtH5::String
GtH5::getAttributeName(Attribute const& attr)
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
{
}

hid_t
GtH5::Attribute::id() const
{
    return m_attribute.getId();
}

const H5::H5Location*
GtH5::Attribute::toH5Location() const
{
    return &m_attribute;
}

bool
GtH5::Attribute::isValid() const
{
    return Location::isValid();
}

H5::Attribute const&
GtH5::Attribute::toH5() const
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
    catch (H5::AttributeIException& /*e*/)
    {
        qCritical() << "HDF5: Writing attribute failed! -" << name();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Attribute::doWrite failed! -"
                    << name();
    }
    return false;
}

bool
GtH5::Attribute::doRead(void* data, DataType const& dtype) const
{
    try
    {
        m_attribute.read(dtype.toH5(), data);
        return true;
    }
    catch (H5::AttributeIException& /*e*/)
    {
        qCritical() << "HDF5: Reading attribute failed! -" << name();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Attribute::doRead failed! -"
                    << name();
    }
    return false;
}

bool
GtH5::Attribute::deleteLink()
{
    qDebug() << "HDF5: Deleting attribute...";

    if (!isValid())
    {
        qWarning() << "HDF5: Attribute deletion failed! (attribute is invalid)";
        return false;
    }

    // returns error type
    if (H5Adelete_by_name(file()->id(), path().constData(),
                          name().constData(), H5P_DEFAULT))
    {
        qCritical() << "HDF5: Attribute deletion failed!";
        return false;
    }

    close();
    return true;
}

GtH5::ObjectType
GtH5::Attribute::type() const
{
    return AttributeType;
}

GtH5::String
GtH5::Attribute::name() const
{
    return getAttributeName(*this);
}

void
GtH5::Attribute::close()
{
    m_attribute.close();
}

H5::AbstractDs const&
GtH5::Attribute::toH5AbsDataSet() const
{
    return m_attribute;
}
