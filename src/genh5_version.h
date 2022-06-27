/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 07.04.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_VERSION_H
#define GENH5_VERSION_H

#include "genh5_exports.h"
#include <utility>

#define GENH5_TO_STR_HELPER(x) #x
#define GENH5_TO_STR(x) GENH5_TO_STR_HELPER(x)

#define GENH5_VERSION_MAJOR 1
#define GENH5_VERSION_MINOR 1
#define GENH5_VERSION_PATCH 0
#define GENH5_VERSION_ADDITIONAL ""
#define GENH5_VERSION ((GENH5_VERSION_MAJOR<<16)|\
                      (GENH5_VERSION_MINOR<<8)|\
                      (GENH5_VERSION_PATCH))

#define GENH5_VCHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define GENH5_VERSION_STR \
    GENH5_TO_STR(GENH5_VERSION_MAJOR) "." \
    GENH5_TO_STR(GENH5_VERSION_MINOR) "." \
    GENH5_TO_STR(GENH5_VERSION_PATCH) \
    GENH5_VERSION_ADDITIONAL

namespace GenH5
{

struct Version
{
    int major{}, minor{}, patch{};

    Version() = default;
    // cppcheck-suppress noExplicitConstructor
    Version(int version) :
        major{(version & 0xff0000) >> 16},
        minor{(version & 0x00ff00) >> 8},
        patch{(version & 0x0000ff)}
    {}
    Version(int vMajor, int vMinor, int vPatch) :
        major{vMajor}, minor{vMinor}, patch{vPatch}
    {}

    static Version current()
    {
        return {GENH5_VERSION_MAJOR, GENH5_VERSION_MINOR, GENH5_VERSION_PATCH};
    }

    int toInt() const
    {
        return major << 16 | minor << 8 | patch;
    }
};

} // namespace GenH5

inline bool
operator==(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() == v2.toInt();
}
inline bool
operator!=(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() != v2.toInt();
}
inline bool
operator<=(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() <= v2.toInt();
}
inline bool
operator>=(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() >= v2.toInt();
}
inline bool
operator<(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() < v2.toInt();
}
inline bool
operator>(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() > v2.toInt();
}

#endif // GENH5_VERSION_H
