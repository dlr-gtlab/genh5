/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 13.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_DATA_BASE_H
#define GENH5_DATA_BASE_H

#include "genh5_datatype.h"
#include "genh5_dataspace.h"
#include "genh5_conversion.h"

namespace GenH5
{

namespace details
{

template<typename T>
class AbstractData
{
public:

    AbstractData() = default;
    virtual ~AbstractData() = default;
    AbstractData(AbstractData const&) = default;
    AbstractData(AbstractData&&) = default;
    AbstractData& operator=(AbstractData const&) = default;
    AbstractData& operator=(AbstractData&&) = default;

    using template_type = T;
    using size_type     = size_t;

    // only necessary for when creating compound types
    using compound_names = CompoundNames<traits::comp_size<T>::value>;

    AbstractData(compound_names names) :
        m_typeNames{std::move(names)}
    { }

    /** helper functions for reading and writing to datasets **/
    // dataspace
    virtual DataSpace dataSpace() const = 0;

    // data size
    virtual size_type size() const = 0;

    // resize data
    virtual bool resize(DataSpace const& dspace, DataType const& dtype) = 0;

    // pointer for reading
    virtual void* dataReadPtr() = 0;

    // pointer for writing
    virtual void const* dataWritePtr() const = 0;

    template <size_t N = traits::comp_size<T>::value,
              traits::if_greater_than<N, 0> = true>
    static DataType dataType(compound_names names) noexcept(false)
    {
        return GenH5::dataType<T>(std::move(names));
    }

    virtual DataType dataType() const noexcept(false)
    {
        return GenH5::dataType<T>(m_typeNames);
    }

    /** compound type names **/
    // access names used when generating a compound type
    compound_names const& typeNames() const
    {
        return m_typeNames;
    }

    // set compound type names
    template <size_t N = traits::comp_size<T>::value,
              traits::if_greater_than<N, 0> = true>
    void setTypeNames(compound_names names)
    {
        m_typeNames = std::move(names);
    }

    // set compound type names using datatype
    template <size_t N = traits::comp_size<T>::value>
    void setTypeNames(DataType const& dtype)
    {
        m_typeNames = getTypeNames<N>(dtype);
    }

protected:

    // compound type names
    compound_names m_typeNames{};
};

} // namespace details

} // namespace GenH5

#endif // GENH5_DATA_BASE_H
