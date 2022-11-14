/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_node.h"

#include "genh5_version.h"
#include "genh5_group.h"

#include <QDebug>

namespace GenH5
{
namespace mpl
{

template<class R, class... Args, size_t... Is >
R apply_impl(R(*pf)(Args...),
             std::tuple<Args...>&& args,
             std::index_sequence< Is...>)
{
    return pf( std::forward<Args>( std::get<Is>(args))... );
}

template<class R, class ... Args>
R apply(R(*pf)(Args...),
        std::tuple<Args...> args)
{
    return apply_impl(pf, std::move(args),
                      std::make_index_sequence<sizeof...(Args)>());
}

} // namespace mpl

template <typename...>
class Finally;

template<typename R, typename... Args>
class Finally<R(Args...)>
{
    using FunctionType = R(*)(Args...);

public:

    constexpr explicit Finally(FunctionType function, Args... args) :
        m_f{function}, m_args{args...}
    { }

    ~Finally()
    {
        mpl::apply(m_f, std::move(m_args));
    }

    Finally(Finally const&) = delete;
    Finally(Finally&&) = delete;
    Finally& operator=(Finally const&) = delete;
    Finally& operator=(Finally&&) = delete;

private:

    FunctionType m_f;
    std::tuple<Args...> m_args;
};

} // namespace GenH5

GenH5::DataSet
GenH5::NodeInfo::toDataSet(Group const& parent) const noexcept(false)
{
    return parent.openDataset(path);
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
        return std::make_unique<DataSet>(parent.openDataset(path));
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

GenH5::Node::Node(std::shared_ptr<File> file) noexcept :
    Location{std::move(file)}
{ }

H5::H5Location const*
GenH5::Node::toH5Location() const noexcept
{
    return toH5Object();
}

bool
GenH5::Node::hasAttribute(String const& name) const
{
    try
    {
        return isValid() && toH5Object()->attrExists(name.constData());
    }
    catch (H5::Exception const& /*e*/)
    {
        qCritical() << "HDF5: [EXCEPTION] Node::hasAttribute";
        return false;
    }
}

GenH5::Attribute
GenH5::Node::createAttribute(String const& name,
                            DataType const& dtype,
                            DataSpace const& dspace) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw AttributeException{"Creating attribute failed (invalid parent)"};
    }

    // create new attribute!
    if (!parent.hasAttribute(name))
    {
        try
        {
            auto attr = parent.toH5Object()->createAttribute(
                        name.constData(), dtype.toH5(), dspace.toH5());
            return Attribute{parent.file(), std::move(attr)};
        }
        catch (H5::AttributeIException const& e)
        {
            throw AttributeException{e.getCDetailMsg()};
        }
        catch (H5::Exception const& e)
        {
            qCritical() << "HDF5: [EXCEPTION] Node::createAttribute";
            throw AttributeException{e.getCDetailMsg()};
        }
    }

    // open existing attribute
    auto attr = openAttribute(name);

    // check if memory layout is equal
    if (attr.dataType() == dtype && attr.dataSpace() == dspace)
    {
        return attr;
    }

    // attribute cannot be resized and must be deleted
    qWarning() << "HDF5: Invalid memory layout! Overwriting attribute! -"
               << name;
    attr.deleteLink();

    return createAttribute(std::move(name), dtype, dspace);
}

GenH5::Attribute
GenH5::Node::openAttribute(String const& name) const noexcept(false)
{
    auto const& parent = *this;

    if (!parent.isValid())
    {
        throw AttributeException{"Opening attribute failed (invalid parent)"};
    }

    try
    {
        auto attr = parent.toH5Object()->openAttribute(name.constData());
        return Attribute{parent.file(), std::move(attr)};
    }
    catch (H5::AttributeIException const& e)
    {
        throw AttributeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Node::openAttribute";
        throw AttributeException{e.getCDetailMsg()};
    }
}

GenH5::Attribute
GenH5::Node::openAttribute(String const& path,
                           String const& name) const noexcept(false)
{
    hid_t attrId = H5Aopen_by_name(id(), path.constData(), name.constData(),
                                   H5P_DEFAULT, H5P_DEFAULT);

    if (!Object::isValid(attrId))
    {
        throw AttributeException{"Opening attribute by name failed"};
    }

    // "finally" block for cleanup
    Finally<herr_t(hid_t)> finally{H5Aclose, attrId};

    try
    {
        H5::Attribute attr = attrId;
        return Attribute{file(), std::move(attr)};
    }
    catch (H5::AttributeIException const& e)
    {
        throw AttributeException{e.getCDetailMsg()};
    }
    catch (H5::Exception const& e)
    {
        qCritical() << "HDF5: [EXCEPTION] Node::openAttribute - by name";
        throw AttributeException{e.getCDetailMsg()};
    }
}

GenH5::String
GenH5::Node::versionAttributeName()
{
    return QByteArrayLiteral("GENH5_VERSION");
}

#ifndef GENH5_NO_DEPRECATED_SYMBOLS
GenH5::String
GenH5::Node::versionAttrName()
{
    return versionAttributeName();
}
#endif

bool
GenH5::Node::hasVersionAttribute(String const& string) const
{
    return hasAttribute(string);
}

bool
GenH5::Node::createVersionAttribute() const noexcept(false)
{
    writeVersionAttribute();
    return true;
}

GenH5::Node const&
GenH5::Node::writeVersionAttribute(String const& string,
                                   Version version) const noexcept(false)
{
    auto attr = createAttribute(string, dataType<Version>(), DataSpace::Scalar);
    if (!attr.write(&version))
    {
        throw AttributeException{"Failed to write version Attribute!"};
    }
    return *this;
}

GenH5::Version
GenH5::Node::readVersionAttribute(String const& string) const noexcept(false)
{
    Version version{-1, -1, -1};
    auto attr = openAttribute(string);
    if (attr.dataType() != DataType::Version ||
        attr.dataSpace() != DataSpace::Scalar)
    {
        throw AttributeException{"Invalid version Attribute format!"};
    }
    if (!attr.read(&version))
    {
        throw AttributeException{"Failed to read version Attribute!"};
    }
    return version;
}

namespace GenH5
{

namespace alg
{

inline AttributeInfo
getAttributeInfo(char const* attrName, H5A_info_t const* attrInfo)
{
    assert(attrName);
    assert(attrInfo);
//    qDebug() << "ATTRIBUTE" << attrName;

    AttributeInfo info;
    info.name = QByteArray{attrName};
    info.corder = attrInfo->corder_valid ? attrInfo->corder : -1;
    return info;
}

inline herr_t
accumulateAttributes(hid_t /*locId*/, char const* attrName,
                     H5A_info_t const* attrInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* nodes = static_cast<Vector<AttributeInfo>*>(dataPtr);
    *nodes << getAttributeInfo(attrName, attrInfo);
    return 0;
}

struct IterForeachData
{
    Node const* parent{};
    AttributeIterationFunction* f{};
};

inline herr_t
foreachAttribute(hid_t /*locId*/, char const* attrName,
                 H5A_info_t const* attrInfo, void* dataPtr)
{
    assert(dataPtr);

    auto* data = static_cast<IterForeachData*>(dataPtr);
    assert(data->parent);
    assert(data->f);

    AttributeInfo info = getAttributeInfo(attrName, attrInfo);

    return (*data->f)(*data->parent, info);
}

} // namespae alg

} // namespace GenH5

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

    alg::IterForeachData data{this, &iterFunction};

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
        throw AttributeException{"Failed to retrieve attribute info!"};
    }
    return alg::getAttributeInfo(name.constData(), &info);
}
