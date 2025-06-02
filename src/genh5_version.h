/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 07.04.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_VERSION_H
#define GENH5_VERSION_H

#include "genh5_exports.h"

#include <utility>

#define GENH5_VERSION_MAJOR 3
#define GENH5_VERSION_MINOR 0
#define GENH5_VERSION_PATCH 4
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

    constexpr Version() = default;
    // cppcheck-suppress noExplicitConstructor
    constexpr Version(int version) :
        major{(version & 0xff0000) >> 16},
        minor{(version & 0x00ff00) >> 8},
        patch{(version & 0x0000ff)}
    {}
    constexpr Version(int vMajor, int vMinor, int vPatch) :
        major{vMajor}, minor{vMinor}, patch{vPatch}
    {}

    /**
     * @brief Returns the version used at compile time of this library
     * @return current version struct
     */
    GENH5_EXPORT static Version current();

    /**
     * @brief Returns the version subrelease/additional info used at compile
     * time of this library
     * @return Current version subrelease
     */
    GENH5_EXPORT static const char* subrelease();

    /**
     * @brief Returns the HDF5 library version used at compile time of this
     * library
     * @return HDF5 version struct
     */
    GENH5_EXPORT static Version hdf5();

    /**
     * @brief Returns the HDF5 library version subrelease used at compile time
     * of this library
     * @return HDf5 version subrelease
     */
    GENH5_EXPORT static const char* subreleaseHdf5();

    /**
     * @brief Transforms the version to struct to an integer
     * @return version as int
     */
    constexpr int toInt() const { return major << 16 | minor << 8 | patch; }
};

} // namespace GenH5

constexpr inline bool
operator==(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() == v2.toInt();
}
constexpr inline bool
operator!=(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() != v2.toInt();
}
constexpr inline bool
operator<=(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() <= v2.toInt();
}
constexpr inline bool
operator>=(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() >= v2.toInt();
}
constexpr inline bool
operator<(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() < v2.toInt();
}
constexpr inline bool
operator>(GenH5::Version const& v1, GenH5::Version const& v2)
{
    return v1.toInt() > v2.toInt();
}

#endif // GENH5_VERSION_H
