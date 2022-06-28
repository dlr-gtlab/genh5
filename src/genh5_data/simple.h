/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 23.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef SIMPLE_H
#define SIMPLE_H

#include "genh5_data/base.h"

namespace GenH5
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

} // namespace GenH5

#endif // SIMPLE_H
