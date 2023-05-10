/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef H5TESTHELPER_H
#define H5TESTHELPER_H

#define h5TestHelper (TestHelper::instance())

#include <QDebug>
#include <QVector>
#include <QRandomGenerator>

#include "genh5_utils.h"

template<typename... Ts>
QDebug operator<<(QDebug s, std::tuple<Ts...> const& tuple)
{
    QDebugStateSaver saver(s);
    s.nospace() << "tuple(";
    GenH5::mpl::static_for<sizeof...(Ts)-1>([&](auto const idx){
        s << std::get<idx>(tuple) << ", ";
    });
    s << std::get<sizeof...(Ts)-1>(tuple);
    s << ")";
    return s;
}

template<typename... Ts>
QDebug operator<<(QDebug s, std::string const& str)
{
    return s << str.c_str();
}

class QDir;
class QString;

class TestHelper
{
public:
    /**
     * @brief instance
     * @return
     */
    static TestHelper* instance();

    GenH5::String newFilePath() const;

    QString tempPath() const;

    void reset();

    /**
     * @brief generates a random string list (uuids)
     * @param length length of the list
     * @return list
     */
    QStringList randomStringList(int length) const;

    /**
     * @brief generates a random byte array list (uuids)
     * @param length length of the list
     * @return list
     */
    GenH5::Vector<QByteArray> randomByteArrays(int length) const;

    /**
     * @brief generates a random data vector
     * @tparam T type
     * @param length length of the list
     * @return list
     */
    template<typename T = double>
    GenH5::Vector<T> randomDataVector(int length)
    {
        QVector<T> retVal;
        retVal.reserve(length);

        for (int i = 0; i < length; ++i)
        {
            retVal.append(static_cast<T>(
                              QRandomGenerator::global()->generateDouble()*10));
        }

        return retVal;
    }

    /**
     * @brief generates a linear data vector
     * @tparam T type
     * @param length length of the list
     * @return list
     */
    template<typename T = double>
    GenH5::Vector<T> linearDataVector(int length, T start = 0, T step = 1)
    {
        QVector<T> retVal;
        retVal.reserve(length);

        T value = start;
        for (int i = 0; i < length; ++i)
        {
            retVal.append(value);
            value += step;
        }

        return retVal;
    }

private:

    TestHelper() { reset(); }
};

#endif // H5TESTHELPER_H
