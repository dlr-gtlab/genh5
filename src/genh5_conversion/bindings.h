/* 
 * GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#ifndef GENH5_CONVERSION_BINDINGS_H
#define GENH5_CONVERSION_BINDINGS_H

#include "genh5_conversion/defaults.h"

#include <string>
#include <QString>
#include <QByteArray>

/** STL **/
GENH5_DECLARE_CONVERSION_TYPE(std::string, char*);
GENH5_DECLARE_BUFFER_TYPE(std::string, QByteArray);
GENH5_DECLARE_DATATYPE(std::string, DataType::VarString());

namespace GenH5
{

GENH5_DECLARE_CONVERSION(std::string const&, value, buffer)
{
    buffer.push_back(value.c_str());
    return buffer.back().data();
}

} // namespace GenH5

/** QT **/
GENH5_DECLARE_CONVERSION_TYPE(QByteArray, char*);
GENH5_DECLARE_BUFFER_TYPE(QByteArray, QByteArray);
GENH5_DECLARE_DATATYPE(QByteArray, DataType::VarString());

GENH5_DECLARE_CONVERSION_TYPE(QString, char*);
GENH5_DECLARE_BUFFER_TYPE(QString, QByteArray);
GENH5_DECLARE_DATATYPE(QString, DataType::VarString());

// experimental
#ifdef GENH5_USE_EXTENDED_QT_BINDINGS
#include <QPoint>
#include <QPointF>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);
GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    static_assert (sizeof(T) * 2 == sizeof(QPoint),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T>({"xp", "yp"});
};

GENH5_DECLARE_IMPLICIT_CONVERSION(QPointF);
GENH5_DECLARE_DATATYPE_IMPL(QPointF)
{
    using T = decltype (std::declval<QPointF>().x());
    static_assert (sizeof(T) * 2 == sizeof(QPointF),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T>({"xp", "yp"});
};

GENH5_DECLARE_IMPLICIT_CONVERSION(QVector2D);
GENH5_DECLARE_DATATYPE_IMPL(QVector2D)
{
    using T = decltype (std::declval<QVector2D>().x());
    static_assert (sizeof(T) * 2 == sizeof(QVector2D),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T>({"x", "y"});
};

GENH5_DECLARE_IMPLICIT_CONVERSION(QVector3D);
GENH5_DECLARE_DATATYPE_IMPL(QVector3D)
{
    using T = decltype (std::declval<QVector3D>().x());
    static_assert (sizeof(T) * 3 == sizeof(QVector3D),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T, T>({"x", "y", "z"});
};

GENH5_DECLARE_IMPLICIT_CONVERSION(QVector4D);
GENH5_DECLARE_DATATYPE_IMPL(QVector4D)
{
    using T = decltype (std::declval<QVector4D>().x());
    static_assert (sizeof(T) * 4 == sizeof(QVector4D),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T, T, T>({"x", "y", "z", "w"});
};
#endif

namespace GenH5
{

GENH5_DECLARE_CONVERSION(QByteArray, value, buffer)
{
    buffer.push_back(std::move(value));
    return buffer.back().data();
}

GENH5_DECLARE_CONVERSION(QString const&, value, buffer)
{
    buffer.push_back(value.toUtf8());
    return buffer.back().data();
}

} // namespace GenH5

#endif // GENH5_CONVERSION_BINDINGS_H
