/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 15.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_TYPES_H
#define GENH5_TYPES_H

#include "genh5_mpl.h"

#include <tuple>
#include <array>
#include <QVector>

#include "H5public.h"

namespace GenH5
{

// alias for vector type
template <typename T>
using Vector = QVector<T>;

// alias for strings
using String = std::string;

// alias for dimension vector type
using Dimensions = Vector<hsize_t>;

// alias for a compound type
template <typename ...Ts>
using Comp = std::tuple<Ts...>;

// alias for a reverse compound type
template <typename ...Ts>
using RComp = mpl::reversed_comp_t<Comp<Ts...>>;

// alias for a var len type
template<typename T>
using VarLen = Vector<T>;

// alias for a fixed length array type
template<typename T, size_t N>
using Array = std::array<T, N>;

// alias for compound member names
template <size_t N>
using CompoundNames = std::array<String, N>;

} // namespace GenH5

#endif // GENH5_TYPES_H
