/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5attribute.h"
#include "gt_h5node.h"
#include "gt_h5file.h"

#include <QDebug>


GtH5Attribute::GtH5Attribute() = default;

GtH5Attribute
GtH5Attribute::create(GtH5Node const& parent,
                      QByteArray const& name,
                      GtH5DataType const& dtype,
                      GtH5DataSpace const& dspace)
{
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
            qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::create failed! -"
                        << name;
            return {};
        }

        return GtH5Attribute(parent.file(), attr, name);
    }

    // open existing attribute
    GtH5Attribute attr = open(parent, name);

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

    return create(parent, name, dtype, dspace);
}

GtH5Attribute
GtH5Attribute::open(GtH5Node const& parent,
                    QByteArray const& name)
{
    H5::Attribute attr;
    try
    {
        attr = parent.toH5Object()->openAttribute(name.constData());
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Opening attribute failed! -" << name;
        return GtH5Attribute();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::open failed! -"
                    << name;
        return GtH5Attribute();
    }

    return GtH5Attribute(parent.file(), attr, name);
}

GtH5Attribute::GtH5Attribute(std::shared_ptr<GtH5File> file,
                             H5::Attribute const& attr,
                             QByteArray const& name) :
    GtH5Location(std::move(file), name),
    GtH5AbtsractDataSet(GtH5DataType(attr.getDataType()),
                        GtH5DataSpace(attr.getSpace())),
    m_attribute(attr)
{
    if (m_name.isEmpty())
    {
        m_name = getAttrName(*this);
    }
}

GtH5Attribute::GtH5Attribute(GtH5Attribute const& other) :
    GtH5Location(other),
    GtH5AbtsractDataSet(other),
    m_attribute{other.m_attribute}
{
//    qDebug() << "GtH5Attribute::copy";
}

GtH5Attribute::GtH5Attribute(GtH5Attribute&& other) noexcept :
    GtH5Location(std::move(other)),
    GtH5AbtsractDataSet(std::move(other)),
    m_attribute{std::move(other.m_attribute)}
{
//    qDebug() << "GtH5Attribute::move";
}

GtH5Attribute&
GtH5Attribute::operator=(GtH5Attribute const& other)
{
//    qDebug() << "GtH5Attribute::copy=";
    auto dset{other};
    swap(dset);
    return *this;
}

GtH5Attribute&
GtH5Attribute::operator=(GtH5Attribute&& other) noexcept
{
//    qDebug() << "GtH5Attribute::move=";
    swap(other);
    return *this;
}

int64_t
GtH5Attribute::id() const
{
    return m_attribute.getId();
}

const H5::H5Location*
GtH5Attribute::toH5Location() const
{
    return &m_attribute;
}

bool
GtH5Attribute::isValid() const
{
    return GtH5Location::isValid();
}

H5::Attribute
GtH5Attribute::toH5() const
{
    return m_attribute;
}

bool
GtH5Attribute::doWrite(void const* data) const
{
    try
    {
        m_attribute.write(m_datatype.toH5(), data);
        return true;
    }
    catch (H5::AttributeIException& /*e*/)
    {
        qCritical() << "HDF5: Writing attribute failed! -" << m_name;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::doWrite failed! -"
                    << m_name;
    }
    return false;
}

bool
GtH5Attribute::doRead(void* data) const
{
    try
    {
        m_attribute.read(m_datatype.toH5(), data);
        return true;
    }
    catch (H5::AttributeIException& /*e*/)
    {
        qCritical() << "HDF5: Reading attribute failed! -" << m_name;
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::doRead failed! -"
                    << m_name;
    }
    return false;
}

bool
GtH5Attribute::deleteLink()
{
    qDebug() << "HDF5: Deleting attribute...";

    if (!this->isValid())
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

    this->close();
    return true;
}

GtH5Location::ObjectType
GtH5Attribute::type() const
{
    return GtH5Location::ObjectType::Attribute;
}

void
GtH5Attribute::close()
{
    m_attribute.close();
}

void
GtH5Attribute::swap(GtH5Attribute& other) noexcept
{
    using std::swap;
    GtH5Location::swap(other);
    GtH5AbtsractDataSet::swap(other);
    swap(m_attribute, other.m_attribute);
}

void
swap(GtH5Attribute& first, GtH5Attribute& other) noexcept
{
    first.swap(other);
}
