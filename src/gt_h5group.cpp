/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5group.h"
#include "gt_h5file.h"

#include <QDebug>

//GtH5Group::GtH5Group(const GtH5Node& parent,
//                     const QString& name,
//                     AccessFlag mode) :
//    GtH5Group(parent, name.toUtf8(), mode)
//{
//}

//GtH5Group::GtH5Group(const GtH5Node& parent,
//                     const QByteArray& name,
//                     AccessFlag mode)
//{
//    if (!parent.isValid())
//    {
//        return;
//    }

//    m_name = name;
//    m_file = parent.file();

//    // create new group
//    if (mode == AccessFlag::Create || !parent.exists(name))
//    {
//        try
//        {
//            m_group = parent.toH5Object()->createGroup(name.constData());
//        }
//        catch (H5::GroupIException e)
//        {
//            qCritical() << "HDF5: Creating group failed!";
//        }
//        catch(H5::Exception e)
//        {
//            qCritical() << "HDF5: [EXCEPTION] GtH5Group::GtH5Group failed!";
//        }
//        return;
//    }

//    // open existing group
//    try
//    {
//        m_group = parent.toH5Object()->openGroup(name.constData());
//    }
//    catch (H5::GroupIException e)
//    {
//        qCritical() << "HDF5: Opening group failed!";
//    }
//    catch(H5::Exception e)
//    {
//        qCritical() << "HDF5: [EXCEPTION] GtH5Group::GtH5Group failed!";
//    }
//}

GtH5Group
GtH5Group::create(const GtH5Node& parent,
                  const QByteArray& name,
                  GtH5Group::AccessFlag flag)
{
    if (!parent.isValid())
    {
        return GtH5Group();
    }

    // create new group
    if (flag == AccessFlag::Create || !parent.exists(name))
    {
        H5::Group group;
        try
        {
            group = parent.toH5Object()->createGroup(name.constData());
        }
        catch (H5::GroupIException e)
        {
            qCritical() << "HDF5: Creating group failed!";
        }
        catch(H5::Exception e)
        {
            qCritical() << "HDF5: [EXCEPTION] GtH5Group::create failed!";
        }

        return GtH5Group(*parent.file(), group, name);
    }

    // open existing group
    return open(parent, name);
}

GtH5Group
GtH5Group::open(const GtH5Node &parent,
                const QByteArray &name)
{
    if (!parent.isValid())
    {
        return GtH5Group();
    }

    H5::Group group;
    try
    {
        group = parent.toH5Object()->openGroup(name.constData());
    }
    catch (H5::GroupIException e)
    {
        qCritical() << "HDF5: Opening group failed!";
        return GtH5Group();
    }
    catch(H5::Exception e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Group::open failed!";
        return GtH5Group();
    }

    return GtH5Group(*parent.file(), group, name);
}

GtH5Group::GtH5Group(GtH5File& file)
{
    if (!file.isValid())
    {
        return;
    }

    m_file = &file;
    m_name = QByteArrayLiteral("/");

    try
    {
        m_group = file.toH5().openGroup(m_name.constData());
    }
    catch (H5::GroupIException e)
    {
        qCritical() << "HDF5: Opening group failed!";
    }
    catch(H5::Exception e)
    {
        qCritical() << "HDF5: [EXCEPTION] GtH5Group::GtH5Group failed!";
    }
}

GtH5Group::GtH5Group(GtH5File& file,
                     const H5::Group& group,
                     const QByteArray& name) :
    m_group(group)
{
    m_file = &file;
    m_name = name.isEmpty() ? getObjectName(*this) : name;
}

int64_t
GtH5Group::id() const
{
    return m_group.getId();
}

bool
GtH5Group::deleteLink()
{
    return false;
}

GtH5Location::ObjectType
GtH5Group::type() const
{
    return GtH5Location::ObjectType::Group;
}

const H5::H5Object*
GtH5Group::toH5Object() const
{
    return &m_group;
}

H5::Group
GtH5Group::toH5() const
{
    return m_group;
}

void
GtH5Group::close()
{
    m_group.close();
}

