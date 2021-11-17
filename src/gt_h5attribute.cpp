/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5attribute.h"
#include "gt_h5leaf.h"
#include "gt_h5file.h"

#include <QDebug>

GtH5Attribute
GtH5Attribute::create(const GtH5Leaf& parent,
                      const QByteArray& name,
                      const GtH5DataType& dtype,
                      const GtH5DataSpace& dspace,
                      AccessFlag flag)
{
    // create new attribute!
    if (flag == AccessFlag::Create || !parent.hasAttribute(name))
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
            qCritical() << "HDF5: Creating attribute failed!";
            return GtH5Attribute();
        }
        catch (H5::Exception& /*e*/)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::create failed!";
            return GtH5Attribute();
        }

        return GtH5Attribute(*parent.file(), attr, name);
    }

    // open existing attribute
    GtH5Attribute attr = open(parent, name);

    // check if memory layout is equal
    if (attr.dataType() == dtype && attr.dataSpace() == dspace)
    {
        return attr;
    }

    // attribute cannot be resized and must be deleted
    if (flag != CreateOverwrite)
    {
        qDebug() << "HDF5: Overwriting attribute failed! "
                    "(invalid memory layout)";
        return GtH5Attribute();
    }

    attr.deleteLink();

    return create(parent, name, dtype, dspace, Create);
}

GtH5Attribute
GtH5Attribute::open(const GtH5Leaf& parent,
                    const QByteArray& name)
{
    H5::Attribute attr;
    try
    {
        attr = parent.toH5Object()->openAttribute(name.constData());
    }
    catch (H5::DataSetIException& /*e*/)
    {
        qCritical() << "HDF5: Opening attribute failed!";
        return GtH5Attribute();
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::open failed!";
        return GtH5Attribute();
    }

    return GtH5Attribute(*parent.file(), attr, name);
}

GtH5Attribute::GtH5Attribute(GtH5File& file,
                             const H5::Attribute& attr,
                             const QByteArray& name) :
    m_attribute(attr)
{
    m_file = &file;
    m_datatype  = GtH5DataType(attr.getDataType());
    m_dataspace = GtH5DataSpace(attr.getSpace());
    m_name = name.isEmpty() ? getAttrName(*this) : name;
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
    return GtH5Location::isValid(); // && m_datatype.isValid() && m_dataspace.isValid();
}

H5::Attribute
GtH5Attribute::toH5() const
{
    return m_attribute;
}

bool
GtH5Attribute::doWrite(void* data) const
{
    try
    {
        m_attribute.write(m_datatype.toH5(), data);
        return true;
    }
    catch (H5::AttributeIException& /*e*/)
    {
        qCritical() << "HDF5: Writing attribute failed!";
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::doWrite failed!";
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
        qCritical() << "HDF5: Reading attribute failed!";
    }
    catch (H5::Exception& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Attribute::doRead failed!";
    }
    return false;
}

bool
GtH5Attribute::deleteLink()
{
    qDebug() << "HDF5: GtH5Attribute::delete";

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
