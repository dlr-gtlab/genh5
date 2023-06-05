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
#include "genh5_finally.h"

#include "H5Apublic.h"
#include "H5Ppublic.h"

GenH5::Reference::Reference() = default;

GenH5::Reference::Reference(H5R_ref_t ref) noexcept :
    m_ref{ref}
{ }

GenH5::Reference::Reference(Alignment data) noexcept
{
    m_ref.u.align = data;
}

GenH5::Reference::Reference(String buffer)  noexcept(false)
{
    if (buffer.size() != bufferSize)
    {
        throw ReferenceException{
            GENH5_MAKE_EXECEPTION_STR() "invalid reference format (" +
            std::to_string(buffer.size()) + " vs. " +
            std::to_string(bufferSize) + " expected elements)"
        };
    }

    std::move(std::begin(buffer), std::end(buffer), std::begin(m_ref.u.__data));
}

GenH5::Reference::Reference(Location const& location) noexcept(false)
{
    m_ref.u.align = 0;

    if (!location.isValid())
    {
        throw ReferenceException{
            GENH5_MAKE_EXECEPTION_STR()
            "Referencing location failed (invalid id)"
        };
    }

    auto const& path = location.path();

    herr_t error;
    if (classType(location.id()) == H5I_ATTR)
    {
        auto const& name = location.name();
        error = H5Rcreate_attr(location.file().id(), path.data(), name.data(),
                               H5P_DEFAULT, &m_ref);
    }
    else
    {
        error = H5Rcreate_object(location.file().id(), path.data(),
                                 H5P_DEFAULT, &m_ref);
    }

    if (error < 0)
    {
        throw ReferenceException{
            GENH5_MAKE_EXECEPTION_STR() "Referencing location '" +
            location.path() + "' failed"
        };
    }

    // the file ref counter is incremented when creating a reference, however
    // this is not necessary. Therefore we decrement the ref count
    if (isValid())
    {
        H5Idec_ref(location.file().id());
    }
}

bool
GenH5::Reference::isValid() const noexcept
{
    return alignment() > 0;
}

GenH5::String
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
//    auto ref = m_ref;
//    hid_t group = H5Ropen_object(&ref, H5P_DEFAULT, H5P_DEFAULT);
    hid_t group = H5Rdereference(file.root().id(), H5P_DEFAULT,
                                 H5R_OBJECT, &m_ref);
    if (group < 0)
    {
        throw ReferenceException{
            GENH5_MAKE_EXECEPTION_STR() "Dereferencing group failed"
        };
    }

    auto cleanup = finally(H5Oclose, group);
    Q_UNUSED(cleanup)

    // access shared file in root group as the local one may not be the same
    return Group{group};
}

GenH5::DataSet
GenH5::Reference::toDataSet(File const& file) const noexcept(false)
{
//    auto ref = m_ref;
//    hid_t dset = H5Ropen_object(&ref, H5P_DEFAULT, H5P_DEFAULT);
    hid_t dset = H5Rdereference(file.root().id(), H5P_DEFAULT,
                                H5R_OBJECT, &m_ref);
    if (dset < 0)
    {
        throw ReferenceException{
            GENH5_MAKE_EXECEPTION_STR() "Dereferencing dataset failed"
        };
    }

    auto cleanup = finally(H5Oclose, dset);
    Q_UNUSED(cleanup)

    // access shared file in root group as the local one may not be the same
    return DataSet{dset};
}

GenH5::Attribute
GenH5::Reference::toAttribute(File const& file) const noexcept(false)
{
    Q_UNUSED(file)

    auto ref = m_ref;
    hid_t attr = H5Ropen_attr(&ref, H5P_DEFAULT, H5P_DEFAULT);
//    hid_t attr = H5Rdereference(file.root().id(), H5P_DEFAULT, H5R_OBJECT, &m_ref);
    if (attr < 0)
    {
        throw ReferenceException{
            GENH5_MAKE_EXECEPTION_STR() "Dereferencing attribute failed"
        };
    }

    auto cleanup = finally(H5Aclose, attr);
    Q_UNUSED(cleanup)

    // access shared file in root group as the local one may not be the same
    return Attribute{attr};
}

