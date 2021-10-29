/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gt_h5abtsractdataset.h"

bool
GtH5AbtsractDataSet::write(void* data) const
{
    if (data == Q_NULLPTR)
    {
        return false;
    }

    return doWrite(data);
}

bool
GtH5AbtsractDataSet::read(void* data) const
{
    if (data == Q_NULLPTR)
    {
        return false;
    }

    return doRead(data);
}

GtH5DataType
GtH5AbtsractDataSet::dataType() const
{
    return m_datatype;
}

GtH5DataSpace
GtH5AbtsractDataSet::dataSpace() const
{
    return m_dataspace;
}
