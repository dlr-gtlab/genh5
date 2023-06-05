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
template<typename Vec>
Vec concat(Vec const& a)
{
    return a;
}

template<typename Vec, typename ...VecOther>
Vec concat(Vec a, Vec const& b, VecOther const& ...other)
{
    a.insert(a.end(), b.begin(), b.end());
    return concat(a, other...);
}

template<typename Vec, typename T>
bool contains(Vec const& a, T const& b)
{
    return std::find(a.begin(), a.end(), b) != std::end(a);
}

template<typename Vec>
Vec mid(Vec const& d, size_t pos, hssize_t len = -1)
{
    auto start = std::next(std::cbegin(d), pos);
    auto dist = std::distance(start, std::end(d));
    return Vec{start, std::next(start, len < 0 ? dist : std::min(len, dist))};
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
        GenH5::Vector<T> retVal;
        retVal.reserve(length);

        for (size_t i = 0; i < length; ++i)
        {
            retVal.push_back(static_cast<T>(
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
        GenH5::Vector<T> retVal;
        retVal.reserve(length);

        T value = start;
        for (size_t i = 0; i < length; ++i)
        {
            retVal.push_back(value);
            value += step;
        }

        return retVal;
    }

private:

    TestHelper() { reset(); }
};

#endif // H5TESTHELPER_H
