/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 23.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef SIMPLE_H
#define SIMPLE_H

#include "gth5_data/base.h"

namespace GtH5
{

template<typename T>
class Data : public AbstractData<T>
{
    using base_class =  AbstractData<T>;
public:
    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];
};

template <typename Container>
inline auto
makeData(Container&& c)
{
    return Data<traits::value_t<Container>>{c};
}

} // namespace GtH5

#ifndef GTH5_NO_DEPRECATED_SYMBOLS
template<typename... Ts>
class GtH5Data;

template<typename T>
class GtH5Data<T> : public GtH5::AbstractData<T>
{
    using base_class = GtH5::AbstractData<T>;
public:
    using base_class::base_class;
    using base_class::operator=;
    using base_class::operator[];

    auto data() { return base_class::c(); }
    auto data() const { return base_class::c(); }
};
#endif

#endif // SIMPLE_H
