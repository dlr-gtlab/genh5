#ifndef GENH5_CONVERSION_BINDINGS_H
#define GENH5_CONVERSION_BINDINGS_H

#include "defaults.h"

#include <QString>
#include <QByteArray>

/* QT */
GENH5_DECLARE_CONVERSION_TYPE(QString, char*);
GENH5_DECLARE_CONVERSION_TYPE(QByteArray, char*);

GENH5_DECLARE_BUFFER_TYPE(QByteArray, QByteArray);
GENH5_DECLARE_BUFFER_TYPE(QString, QByteArray);

GENH5_DECLARE_DATATYPE(QByteArray, DataType::VarString);
GENH5_DECLARE_DATATYPE(QString, DataType::VarString);

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
