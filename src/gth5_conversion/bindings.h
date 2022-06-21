#ifndef GTH5_CONVERSION_BINDINGS_H
#define GTH5_CONVERSION_BINDINGS_H

#include "defaults.h"

#include <QString>
#include <QByteArray>

/* QT */
GTH5_DECLARE_CONVERSION_TYPE(QString, char*);
GTH5_DECLARE_CONVERSION_TYPE(QByteArray, char*);

GTH5_DECLARE_BUFFER_TYPE(QByteArray, QByteArray);
GTH5_DECLARE_BUFFER_TYPE(QString, QByteArray);

GTH5_DECLARE_DATATYPE(QByteArray, DataType::VarString);
GTH5_DECLARE_DATATYPE(QString, DataType::VarString);

namespace GtH5
{

GTH5_DECLARE_CONVERSION(QByteArray, value, buffer)
{
    buffer.push_back(std::move(value));
    return buffer.back().data();
}

GTH5_DECLARE_CONVERSION(QString const&, value, buffer)
{
    buffer.push_back(value.toUtf8());
    return buffer.back().data();
}

} // namespace GtH5

#endif // GTH5_CONVERSION_BINDINGS_H
