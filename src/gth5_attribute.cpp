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

namespace
{
    GtH5::DataType getDataType(H5::Attribute const& attr)
    {
        if (!GtH5::Object::isValid(attr.getId()))
        {
            return {};
        }

        try
        {
        return GtH5::DataType{attr.getDataType()};
        }
        catch (H5::Exception& /*e*/)
        {
            return {};
        }
    }

    GtH5::DataSpace getDataSpace(H5::Attribute const& attr)
    {
        if (!GtH5::Object::isValid(attr.getId()))
        {
            return {};
        }

        try
        {
        return GtH5::DataSpace{attr.getSpace()};
        }
        catch (H5::Exception& /*e*/)
        {
            return {};
        }
    }
}

GtH5::Attribute::Attribute() = default;

GtH5::Attribute::Attribute(std::shared_ptr<File> file,
                           H5::Attribute attr,
                           String name) :
    Location{std::move(file), std::move(name)},
    AbstractDataSet{getDataType(attr),
                    getDataSpace(attr)},
    m_attribute(std::move(attr))
{
    if (m_name.isEmpty())
    {
        m_name = GtH5::getAttributeName(*this);
    }
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
        qCritical() << "HDF5: Writing attribute failed! -" << m_name;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Attribute::doWrite failed! -"
                    << m_name;
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
        qCritical() << "HDF5: Reading attribute failed! -" << m_name;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5::Attribute::doRead failed! -"
                    << m_name;
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
                          m_name.constData(), H5P_DEFAULT))
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

void
GtH5::Attribute::close()
{
    m_attribute.close();
}
