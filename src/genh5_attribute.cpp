/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_attribute.h"
#include "genh5_node.h"
#include "genh5_file.h"
#include "genh5_exception.h"
#include "genh5_private.h"

#include "H5Ppublic.h"
#include "H5Apublic.h"

GenH5::String
GenH5::getAttributeName(Attribute const& attr) noexcept
{
    return GenH5::details::getName(attr.id(),
                                   [](hid_t id, size_t len, char* data) {
         return H5Aget_name(id, len, data);
    });
}

GenH5::Attribute::Attribute() = default;

GenH5::Attribute::Attribute(hid_t id) :
    m_id(id)
{
    m_id.inc();
}

hid_t
GenH5::Attribute::id() const noexcept
{
    return m_id;
}

bool
GenH5::Attribute::doWrite(void const* data, DataType const& dtype) const
{
    herr_t err = H5Awrite(m_id, dtype.id(), data);

    return err >= 0;
}

bool
GenH5::Attribute::doRead(void* data, DataType const& dtype) const
{
    herr_t err = H5Aread(m_id, dtype.id(), data);

    return err >= 0;
}

void
GenH5::Attribute::deleteLink() noexcept(false)
{
    if (!isValid())
    {
        throw LocationException{
            GENH5_MAKE_EXECEPTION_STR()
            "Deleting attribute failed (invalid id)"
        };
    }

    // returns error type
    if (H5Adelete_by_name(file().id(), path().constData(),
                          name().constData(), H5P_DEFAULT) < 0)
    {
        throw LocationException{
            GENH5_MAKE_EXECEPTION_STR() "Deleting attribute '" +
            path().toStdString() + ":" + name().toStdString() + "' failed"
        };
    }

    close();
}

GenH5::String
GenH5::Attribute::name() const noexcept
{
    return getAttributeName(*this);
}

GenH5::NodeInfo
GenH5::Attribute::nodeInfo() const noexcept(false)
{
    if (!isValid())
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR()
            "Accessing node object failed (invalid attribute object)"
        };
    }

    auto path = this->path();
    if (path == "/") // root
    {
        return NodeInfo{path, IdType::Group};
    }
    return file().root().nodeInfo(std::move(path));
}

void
GenH5::Attribute::close()
{
    m_id.dec();
}

GenH5::DataType
GenH5::Attribute::dataType() const noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to access datatype";

    if (!isValid())
    {
        throw DataTypeException{
            GENH5_MAKE_EXECEPTION_STR() "Accessing datatype failed (invalid id)"
        };
    }

    return details::make<DataType, DataTypeException>([id = m_id](){
        return H5Aget_type(id);
    }, errMsg);
}

GenH5::DataSpace
GenH5::Attribute::dataSpace() const noexcept(false)
{
    static const std::string errMsg =
            GENH5_MAKE_EXECEPTION_STR() "Failed to access dataspace";

    if (!isValid())
    {
        throw DataSpaceException{
            GENH5_MAKE_EXECEPTION_STR()
            "Accessing dataspace failed (invalid id)"
        };
    }

    return details::make<DataSpace, DataSpaceException>([id = m_id](){
        return H5Aget_space(id);
    }, errMsg);
}

void
GenH5::Attribute::swap(Attribute& other) noexcept
{
    using std::swap;
    swap(m_id, other.m_id);
}
