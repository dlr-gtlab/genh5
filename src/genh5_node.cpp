/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_node.h"

#include "genh5_file.h"
#include "genh5_private.h"
#include "genh5_version.h"
#include "genh5_group.h"

#include "H5Apublic.h"
#include "H5Ppublic.h"

GenH5::DataSet
GenH5::NodeInfo::toDataSet(Group const& parent) const noexcept(false)
{
    return parent.openDataSet(path);
}

GenH5::Group
GenH5::NodeInfo::toGroup(Group const& parent) const noexcept(false)
{
    return parent.openGroup(path);
}

std::unique_ptr<GenH5::Node>
GenH5::NodeInfo::toNode(GenH5::Group const& parent) const noexcept(false)
{
    if (isDataSet())
    {
        return std::make_unique<DataSet>(parent.openDataSet(path));
    }
    return std::make_unique<Group>(parent.openGroup(path));
}

GenH5::Attribute
GenH5::AttributeInfo::toAttribute(Node const& parent) const noexcept(false)
{
    return parent.openAttribute(name);
}

GenH5::Attribute
GenH5::AttributeInfo::toAttribute(Node const& object,
                                  String const& path) const noexcept(false)
{
    return object.openAttribute(path, name);
}

GenH5::Node::Node() noexcept = default;

bool
GenH5::Node::hasParent() const noexcept
{
    auto path = this->path();
    return path.size() > 1 && path.lastIndexOf('/') >= 0;
}

GenH5::Group
GenH5::Node::parent() const noexcept(false)
{
    auto path = this->path();
    if (path.isEmpty()) // -> presumably invalid id
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR()
            "Accessing parent failed (invalid group object)"
        };
    }

    auto idx = path.lastIndexOf('/');
    if (path.size() <= 1 || idx < 0)
    {
        throw GroupException{
            GENH5_MAKE_EXECEPTION_STR()
            "Accessing parent failed (no parent)"
        };
    }

    return file().root().openGroup(path.mid(0, idx + 1));
}

bool
GenH5::Node::hasAttribute(String const& name) const noexcept
{
    return isValid() && H5Aexists(id(), name.constData()) > 0;
}

GenH5::Attribute
GenH5::Node::createAttribute(String const& name,
                             DataType const& dtype,
                             DataSpace const& dspace) const noexcept(false)
{
    if (!isValid())
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Creating attribute '" +
            name.toStdString() + "' failed (invalid parent)"
        };
    }

    // create new attribute!
    if (!hasAttribute(name))
    {
        hid_t attr = H5Acreate(id(), name.constData(), dtype.id(), dspace.id(), H5P_DEFAULT, H5P_DEFAULT);
        if (attr < 0)
        {
            throw AttributeException{
                GENH5_MAKE_EXECEPTION_STR() "Failed to create attribute '" +
                name.toStdString() + '\''
            };
        }

        // "finally" block for cleanup
        auto cleanup = finally(H5Aclose, attr);
        Q_UNUSED(cleanup)

        return Attribute{attr};
    }

    // open existing attribute
    auto attr = openAttribute(name);

    // check if memory layout is equal
    if (attr.dataType() == dtype && attr.dataSpace() == dspace)
    {
        return attr;
    }

    // attribute cannot be resized and must be deleted
    log::ErrStream() << GENH5_MAKE_EXECEPTION_STR()
                        "Invalid memory layout, overwriting attribute: " << name;
    attr.deleteLink();

    return createAttribute(std::move(name), dtype, dspace);
}

GenH5::Attribute
GenH5::Node::openAttribute(String const& name) const noexcept(false)
{
    if (!isValid())
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Opening attribute '" +
            name.toStdString() + "' failed (invalid parent)"
        };
    }

    hid_t attr = H5Aopen(id(), name.constData(), H5P_DEFAULT);
    if (attr < 0)
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to open attribute '" +
            name.toStdString() + '\''
        };
    }

    // "finally" block for cleanup
    auto cleanup = finally(H5Aclose, attr);
    Q_UNUSED(cleanup)

    return Attribute{attr};
}

GenH5::Attribute
GenH5::Node::openAttribute(String const& path,
                           String const& name) const noexcept(false)
{
    if (!isValid())
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Opening attribute by path '" +
            path.toStdString() + ':' + name.toStdString() +
            "' failed (invalid parent)"
        };
    }

    hid_t attr = H5Aopen_by_name(id(), path.constData(), name.constData(),
                                 H5P_DEFAULT, H5P_DEFAULT);

    if (attr < 0)
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to open attribute by path '" +
            path.toStdString() + ':' + name.toStdString() + '\''
        };
    }

    // "finally" block for cleanup
    auto cleanup = finally(H5Aclose, attr);
    Q_UNUSED(cleanup)

    return Attribute{attr};
}

GenH5::String
GenH5::Node::versionAttributeName()
{
    return QByteArrayLiteral("GENH5_VERSION");
}

bool
GenH5::Node::hasVersionAttribute(String const& string) const
{
    return hasAttribute(string);
}

GenH5::Node const&
GenH5::Node::writeVersionAttribute(String const& string,
                                   Version version) const noexcept(false)
{
    auto attr = createAttribute(string, dataType<Version>(), DataSpace::Scalar);
    if (!attr.write(&version))
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to write version attribute '" +
            string.toStdString() + '\''
        };
    }
    return *this;
}

GenH5::Version
GenH5::Node::readVersionAttribute(String const& string) const noexcept(false)
{
    Version version{-1, -1, -1};
    Attribute attr = openAttribute(string);

    if (attr.dataType()  != DataType::Version ||
        attr.dataSpace() != DataSpace::Scalar)
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid version attribute format (" +
            string.toStdString() + ')'
        };
    }
    if (!attr.read(&version))
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR() "Failed to read version attribute (" +
            string.toStdString() + ')'
        };
    }
    return version;
}

GenH5::Vector<GenH5::AttributeInfo>
GenH5::Node::findAttributes(IterationIndex iterIndex,
                            IterationOrder iterOrder) const noexcept
{
    return findAttributes(QByteArrayLiteral("."), iterIndex, iterOrder);
}

GenH5::Vector<GenH5::AttributeInfo>
GenH5::Node::findAttributes(String const& path,
                            IterationIndex iterIndex,
                            IterationOrder iterOrder) const noexcept
{
    GenH5::Vector<GenH5::AttributeInfo> attributes;

    hsize_t idx_ = 0;
    H5Aiterate_by_name(id(), path.constData(),
                       static_cast<H5_index_t>(iterIndex),
                       static_cast<H5_iter_order_t>(iterOrder), &idx_,
                       alg::accumulateAttributes, &attributes,
                       H5P_DEFAULT);
    return attributes;
}

herr_t
GenH5::Node::iterateAttributes(AttributeIterationFunction iterFunction,
                               IterationIndex iterIndex,
                               IterationOrder iterOrder) const noexcept
{
    return iterateAttributes(QByteArrayLiteral("."), iterFunction,
                             iterIndex, iterOrder);
}

herr_t
GenH5::Node::iterateAttributes(String const& path,
                               AttributeIterationFunction iterFunction,
                               IterationIndex iterIndex,
                               IterationOrder iterOrder) const noexcept
{
    assert(iterFunction);
    assert(!path.isEmpty());

    alg::IterForeachAttrData data{this, &iterFunction};

    hsize_t idx_ = 0;
    herr_t error = H5Aiterate_by_name(id(), path.constData(),
                                      static_cast<H5_index_t>(iterIndex),
                                      static_cast<H5_iter_order_t>(iterOrder),
                                      &idx_, alg::foreachAttribute, &data,
                                      H5P_DEFAULT);

    return error;
}

GenH5::AttributeInfo
GenH5::Node::attributeInfo(String const& name) const noexcept(false)
{
    return attributeInfo(QByteArrayLiteral("."), name);
}

GenH5::AttributeInfo
GenH5::Node::attributeInfo(String const& path,
                           String const& name) const noexcept(false)
{
    H5A_info_t info{};
    if (H5Aget_info_by_name(id(), path.constData(),
                            name.constData(), &info, H5P_DEFAULT))
    {
        throw AttributeException{
            GENH5_MAKE_EXECEPTION_STR()
            "Failed to retrieve attribute info for '" +
            path.toStdString() + ':' + name.toStdString() + '\''
        };
    }
    return alg::getAttributeInfo(name.constData(), &info);
}
