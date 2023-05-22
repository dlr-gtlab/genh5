/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 22.5.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_FINALLY_H
#define GENH5_FINALLY_H

#include "genh5_globals.h"
#include "genh5_typetraits.h"

namespace GenH5
{

/**
 * @brief The Finally class.
 * Calls a member function on an object in the destructor. Used for cleaning up.
 */
template <typename Functor>
class Finally
{
public:

    explicit Finally(Functor func) :
        m_func{std::move(func)}
    { }

    // no copy
    Finally(Finally const&) = delete;
    Finally& operator=(Finally const&) = delete;

    // move allowed
    Finally(Finally&& other) :
        m_func{other.m_func},
        m_invoked{other.m_invoked}
    {
        other.clear();
    };

    Finally& operator=(Finally&& other)
    {
        using std::swap; // ADL
        Finally tmp{std::move(other)};
        swap(m_func, tmp.m_func);
        swap(m_invoked, tmp.m_invoked);
        return *this;
    };

    ~Finally() { invoke(); }

    /**
     * @brief Check if function or object is null
     * @return
     */
    bool isNull() const
    {
        return m_invoked;
    }

    /**
     * @brief Calls the cleanup function. Object will go invalid.
     */
    void exec()
    {
        invoke();
        clear();
    }

    /**
     * @brief Clears object
     */
    void clear()
    {
        m_invoked = true;
    }

private:

    // actual function to call
    Functor m_func;

    /// Indicates that method was already
    /// called
    bool m_invoked = false;

    /**
     * @brief Calls the cleanup function.
     */
    void invoke()
    {
        // call cleanup function
        if (!isNull()) m_func();
    }
};

/**
 * @brief Makes a finally object from a lambda
 * @param t object
 * @param func Function to call on cleanup
 */
template<typename Functor, traits::if_not_pointer<Functor> = true>
GENH5_NODISCARD auto finally(Functor func)
{
    return Finally<Functor>{std::move(func)};
}

template<typename R, typename... Args>
GENH5_NODISCARD auto finally(R(*func)(Args...), Args... args)
{
    // construct helper lambda
    return finally(
        [func, args...](){
            func(args...);
        }
    );
}

} // namespace GenH5

#endif // GENH5_FINALLY_H
