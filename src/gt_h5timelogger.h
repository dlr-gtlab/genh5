/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GT_H5TIMELOGGER_H
#define GT_H5TIMELOGGER_H

#include <QDebug>
#include <QElapsedTimer>

/**
 * @brief The GtH5TimeLogger class
 */
class GtH5TimeLogger : public QElapsedTimer
{
public:

    GtH5TimeLogger(const char* string) : info(string)
    {
        start();
    }

    ~GtH5TimeLogger()
    {
        uint us = nsecsElapsed() / 1000;
        qDebug().noquote() << info << "- took"
                           << QString::number(us / 1000.0) << "ms";
    }

private:

    const char* info;
};

#define GTH5_LOG_TIME GtH5TimeLogger __time_logger__(__func__)

#endif // GT_H5LOGGER_H
