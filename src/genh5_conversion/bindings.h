#ifndef GENH5_CONVERSION_BINDINGS_H
#define GENH5_CONVERSION_BINDINGS_H

#include "genh5_conversion/defaults.h"

#include "genh5_bytearray.h"
#include "genh5_version.h"
#include "genh5_reference.h"

/** GENH5 **/
namespace GenH5
{

GENH5_DECLARE_IMPLICIT_CONVERSION(GenH5::Version);
GENH5_DECLARE_IMPLICIT_CONVERSION(GenH5::Reference);

} // namespace GenH5

GENH5_DECLARE_DATATYPE(GenH5::Version, DataType::Version);
GENH5_DECLARE_DATATYPE(GenH5::Reference, DataType::Reference);

/** STL **/
GENH5_DECLARE_CONVERSION_TYPE(std::string, char*);
GENH5_DECLARE_BUFFER_TYPE(std::string, ByteArray);
GENH5_DECLARE_DATATYPE(std::string, DataType::VarString);

namespace GenH5
{

GENH5_DECLARE_CONVERSION(std::string const&, value, buffer)
{
    buffer.push_back(value);
    return buffer.back().data();
}

} // namespace GenH5

#ifdef GENH5_USE_QT_BINDINGS
#include <QString>
#include <QByteArray>

/** QT **/
GENH5_DECLARE_CONVERSION_TYPE(QByteArray, char*);
GENH5_DECLARE_BUFFER_TYPE(QByteArray, ByteArray);
GENH5_DECLARE_DATATYPE(QByteArray, DataType::VarString);

GENH5_DECLARE_CONVERSION_TYPE(QString, char*);
GENH5_DECLARE_BUFFER_TYPE(QString, ByteArray);
GENH5_DECLARE_DATATYPE(QString, DataType::VarString);

// experimental
#ifdef GENH5_USE_EXTENDED_QT_BINDINGS
#include <QPoint>
#include <QPointF>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

namespace GenH5
{

GENH5_DECLARE_IMPLICIT_CONVERSION(QPoint);
GENH5_DECLARE_IMPLICIT_CONVERSION(QPointF);
GENH5_DECLARE_IMPLICIT_CONVERSION(QVector2D);
GENH5_DECLARE_IMPLICIT_CONVERSION(QVector3D);
GENH5_DECLARE_IMPLICIT_CONVERSION(QVector4D);

} // namespace GenH5

GENH5_DECLARE_DATATYPE_IMPL(QPoint)
{
    using T = decltype (std::declval<QPoint>().x());
    static_assert (sizeof(T) * 2 == sizeof(QPoint),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T>({"xp", "yp"});
};

GENH5_DECLARE_DATATYPE_IMPL(QPointF)
{
    using T = decltype (std::declval<QPointF>().x());
    static_assert (sizeof(T) * 2 == sizeof(QPointF),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T>({"xp", "yp"});
};

GENH5_DECLARE_DATATYPE_IMPL(QVector2D)
{
    using T = decltype (std::declval<QVector2D>().x());
    static_assert (sizeof(T) * 2 == sizeof(QVector2D),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T>({"x", "y"});
};

GENH5_DECLARE_DATATYPE_IMPL(QVector3D)
{
    using T = decltype (std::declval<QVector3D>().x());
    static_assert (sizeof(T) * 3 == sizeof(QVector3D),
                   "Unexpected memory layout!");
    return GenH5::dataType<T, T, T>({"x", "y", "z"});
};

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

GENH5_DECLARE_CONVERSION(QByteArray const&, value, buffer)
{
    buffer.push_back(value.data());
    return buffer.back().data();
}

GENH5_DECLARE_CONVERSION(QString const&, value, buffer)
{
    auto&& utf8 = value.toUtf8();
    buffer.push_back(utf8.data());
    return buffer.back().data();
}

} // namespace GenH5

#endif

#endif // GENH5_CONVERSION_BINDINGS_H
