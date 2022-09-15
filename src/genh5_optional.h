/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 20.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_OPTIONAL_H
#define GENH5_OPTIONAL_H

#include <utility>

namespace GenH5
{

/**
 * @brief The Optional class.
 * Wrapper class for a default constructible object. Can be used to check if
 * an object was explicitly constructed.
 */
template<typename Tobject>
class Optional
{
public:

    constexpr Optional() = default;
    // cppcheck-suppress noExplicitConstructor
    constexpr Optional(Tobject const& object) noexcept :
        m_value{object}, m_isDefault{false}
    { }
    // cppcheck-suppress noExplicitConstructor
    constexpr Optional(Tobject&& object) noexcept :
        m_value{std::move(object)}, m_isDefault{false}
    { }

    /**
     * @brief whether the value was default constructed
     * @return false if a custom value was set
     */
    constexpr bool isDefault() const noexcept { return m_isDefault; }

    /**
     * @brief return value. Is always safe
     * @return returns value
     */
    constexpr Tobject const& value() const noexcept { return m_value; }

    void clear() { m_isDefault = true; m_value = {}; }

    constexpr operator Tobject const&() const noexcept { return m_value; }
    constexpr operator Tobject&() noexcept { return m_value; }

    /**
     * @brief operator ->
     * @return pointer to object. Always valid and safe to access (not null)
     */
    constexpr Tobject* operator->() noexcept { return &m_value; }
    constexpr Tobject const* operator->() const noexcept { return &m_value; }

    /**
     * @brief operator *
     * @return reference to object. Always valid and safe to access
     */
    constexpr Tobject& operator*() noexcept { return m_value; }
    constexpr Tobject const& operator*() const noexcept { return m_value; }

    constexpr Optional& operator=(Tobject value) noexcept
    {
        m_value = std::move(value);
        m_isDefault = false;
        return *this;
    }

private:
    /// value
    Tobject m_value{};
    /// tracks if value was default initialized (i.e. not explcitly by user)
    bool m_isDefault{true};
};

}

#endif // GENH5_OPTIONAL_H
