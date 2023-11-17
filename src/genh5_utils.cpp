/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 17.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_utils.h"

#if 0
#include <QDebug>

template <typename Stream>
Stream
printDimensions(Stream stream, GenH5::Dimensions const& dims)
{
    stream << '(';
    for (auto it = dims.cbegin(); it != dims.cend()-1; ++it)
    {
        stream << *it << ", ";
    }
    stream << dims.last();
    return stream << ')';
}

std::ostream&
operator<<(std::ostream& s, GenH5::Dimensions const& d)
{
    return printDimensions<std::ostream&>(s, d);
}

QDebug
operator<<(QDebug s, GenH5::Dimensions const& d)
{
    QDebugStateSaver save{s};
    return printDimensions(s.nospace(), d);
}
#endif
