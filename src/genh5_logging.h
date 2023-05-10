/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 31.1.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_LOGGING_H
#define GENH5_LOGGING_H

#include "genh5_exports.h"
#include "genh5_typedefs.h"

#include <functional>
#include <sstream>

namespace GenH5
{

namespace log
{

using OutputSink = void (std::string const&);

/// registers the output sink provided
GENH5_EXPORT void registerErrorOutputSink(std::function<OutputSink> func);

/// clears the registered output sink
GENH5_EXPORT void clearErrorOutputSink();

/**
 * @brief The ErrStream class. For logging error messages
 */
class ErrStream
{
public:

    ErrStream() {}
    GENH5_EXPORT ~ErrStream();

    std::ostringstream& stream() { return m_stream; }

    template <typename T>
    ErrStream& operator<<(T const&t )
    {
        m_stream << t;
        return *this;
    }

    ErrStream& operator<<(String const& t)
    {
        m_stream << t.data();
        return *this;
    }

private:

    std::ostringstream m_stream;
};

} // namespace log

} // namespace GenH5

#endif // GENH5LOGGING_H
