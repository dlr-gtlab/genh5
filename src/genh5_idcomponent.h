/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 30.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_IDCOMPONENT_H
#define GENH5_IDCOMPONENT_H

#include "genh5_logging.h"
#include "genh5_exports.h"
#include "genh5_exception.h"

#include "H5Ipublic.h"

#include <string>

namespace GenH5
{

/**
 * @brief Increments the id. Same as H5Iinc_ref.
 * @param id Id to increment
 * @return Id ref count if successful else negativ number
 */
GENH5_EXPORT int incId(hid_t id);

/**
 * @brief Decrements the id. Same as H5Idec_ref.
 * @param id Id to decrement
 * @return Id ref count if successful else negativ number
 */
GENH5_EXPORT int decId(hid_t id);

/**
 * @brief Decrements the id. Same as H5Idec_ref.
 * @param id Id to decrement
 * @return Id ref count if successful else negativ number
 */
GENH5_EXPORT bool isValidId(hid_t id);

/**
 * @brief Class type of the id. Same as H5Iget_type.
 * @param id Id
 * @return Id class type
 */
GENH5_EXPORT H5I_type_t classType(hid_t id);

template <H5I_type_t Type>
class GENH5_EXPORT IdComponent
{
public:

    IdComponent(hid_t id = -1) : m_id(id)
    {
        if (id > 0 && classType(id) != Type)
        {
            throw IdComponentException{
                GENH5_MAKE_EXECEPTION_STR() "Invalid id type (" +
                std::to_string(classType(id)) + " vs. " +
                std::to_string(Type) + ')'
            };
        }
    }

    ~IdComponent()
    {
        if (m_id > 0 && isValidId(m_id))
        {
            auto status = decId(m_id);
            if (status < 0)
            {
                log::ErrStream()
                        << GENH5_MAKE_EXECEPTION_STR()
                           "Failed to decrement object id on destructor for '"
                        << m_id << "'. Class type: " << Type;
            }
        }
    }

    IdComponent(IdComponent const& other) : m_id(other.m_id)
    {
        if (m_id > 0 && !(incId(m_id) > 0))
        {
            throw IdComponentException{
                GENH5_MAKE_EXECEPTION_STR()
                "Failed to increment object id on copy"
            };
        }
    }

    IdComponent(IdComponent&& other) : m_id(other.m_id)
    {
        other.m_id = -1;
    }

    IdComponent& operator=(IdComponent const& other)
    {
        IdComponent tmp(other);
        swap(tmp);
        return *this;
    }

    IdComponent& operator=(IdComponent&& other)
    {
        IdComponent tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    void inc()
    {
        if (m_id > 0)
        {
            auto status = incId(m_id);
            if (status < 0)
            {
                throw IdComponentException{
                    GENH5_MAKE_EXECEPTION_STR() "Failed to increment object id"
                };
            }
        }
    }

    void dec()
    {
        if (m_id > 0)
        {
            auto status = decId(m_id);
            if (status < 0)
            {
                throw IdComponentException{
                    GENH5_MAKE_EXECEPTION_STR() "Failed to decrement object id"
                };
            }
            if (status == 0)
            {
                m_id = -1;
            }
        }
    }

    inline operator hid_t() const noexcept { return m_id; }
    inline hid_t get() const noexcept { return m_id; }

    /// swaps all members
    void swap(IdComponent& other) noexcept
    {
        using std::swap;
        swap(m_id, other.m_id);
    }

private:

    hid_t m_id;
};

} // namespace GenH5

template <H5I_type_t Type>
inline void
swap(GenH5::IdComponent<Type>& a, GenH5::IdComponent<Type>& b) noexcept
{
    a.swap(b);
}

#endif // GENH5_IDCOMPONENT_H
