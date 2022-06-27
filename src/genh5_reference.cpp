/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_reference.h"
#include "genh5_file.h"
#include "genh5_attribute.h"
#include "genh5_group.h"
#include "genh5_dataset.h"

#include <QDebug>

GenH5::Reference::Reference() = default;

GenH5::Reference::Reference(H5R_ref_t ref) noexcept :
    m_ref{ref}
{ }

GenH5::Reference::Reference(Alignment data) noexcept
{
    m_ref.u.align = data;
}

GenH5::Reference::Reference(QByteArray buffer)  noexcept(false)
{
    if (buffer.size() != bufferSize)
    {
        qCritical().nospace()
                << "HDF5: invalid buffer format! ("
                << buffer.size() << " vs. " << bufferSize
                << "expected elements)";
        throw ReferenceException{"Invalid buffer format!"};
    }

    std::move(std::begin(buffer), std::end(buffer), std::begin(m_ref.u.__data));
}

GenH5::Reference::Reference(Location const& location) noexcept(false)
{
    m_ref.u.align = 0;

    if (!location.isValid())
    {
        throw ReferenceException{"Referencing location failed "
                                 "(invalid location)"};
    }

    herr_t error;
    if (location.type() == H5I_ATTR)
    {
        error = H5Rcreate_attr(location.file()->id(), location.path(),
                               location.name(), H5P_DEFAULT, &m_ref);
    }
    else
    {
        error = H5Rcreate_object(location.file()->id(), location.path(),
                                 H5P_DEFAULT, &m_ref);
    }

    if (error < 0)
    {
        throw ReferenceException{"Referencing location failed"};
    }

    // the file ref counter is incremented when creating a reference, however
    // this is not necessary. Therefore we decrement the ref count
    if (isValid())
    {
        H5Idec_ref(location.file()->id());
    }
}

bool
GenH5::Reference::isValid() const noexcept
{
    return alignment() > 0;
}

QByteArray
GenH5::Reference::buffer() const noexcept
{
    return {reinterpret_cast<char const*>(m_ref.u.__data), bufferSize};
}

H5R_ref_t const&
GenH5::Reference::toH5() const noexcept
{
    return m_ref;
}

GenH5::Group
GenH5::Reference::toGroup(File const& file) const noexcept(false)
{
    try
    {
        H5::Group group;
        group.dereference(*file.root().toH5Object(), &m_ref);
        // access shared file in root group as the local one may not be the same
        return {file.root().file(), std::move(group)};
    }
    catch (H5::ReferenceException const& e)
    {
        throw ReferenceException{e.getCDetailMsg()};
    }
    catch (H5::GroupIException const& e)
    {
        throw GroupException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Reference::toGroup";
        throw ReferenceException{e.getCDetailMsg()};
    }
}

GenH5::DataSet
GenH5::Reference::toDataSet(File const& file) const noexcept(false)
{
    try
    {
        H5::DataSet dset;
        dset.dereference(*file.root().toH5Object(), &m_ref);
        // access shared file in root group as the local one may not be the same
        return {file.root().file(), std::move(dset)};
    }
    catch (H5::ReferenceException const& e)
    {
        throw ReferenceException{e.getCDetailMsg()};
    }
    catch (H5::DataSetIException const& e)
    {
        throw DataSetException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Reference::toDataSet";
        throw ReferenceException{e.getCDetailMsg()};
    }
}

GenH5::Attribute
GenH5::Reference::toAttribute(File const& file) const noexcept(false)
{
    auto ref = m_ref;
    hid_t id = H5Ropen_attr(&ref, H5P_DEFAULT, H5P_DEFAULT);

    if (id == -1)
    {
        throw ReferenceException{"Dereferencing attribute failed"};
    }

    // access shared file in root group as the local one may not be the same
    return {file.root().file(), id};
}

