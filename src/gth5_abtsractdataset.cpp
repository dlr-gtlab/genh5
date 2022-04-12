/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gth5_abtsractdataset.h"


GtH5AbtsractDataSet::GtH5AbtsractDataSet(GtH5DataType const& dtype,
                                         GtH5DataSpace const& dspace) :
    m_datatype(dtype), m_dataspace(dspace)
{

}

bool
GtH5AbtsractDataSet::write(void const* data) const
{
    if (data == nullptr)
    {
        return false;
    }

    return doWrite(data);
}

bool
GtH5AbtsractDataSet::read(void* data) const
{
    if (data == nullptr)
    {
        return false;
    }

    return doRead(data);
}

GtH5DataType const&
GtH5AbtsractDataSet::dataType() const
{
    return m_datatype;
}

GtH5DataSpace const&
GtH5AbtsractDataSet::dataSpace() const
{
    return m_dataspace;
}

void
GtH5AbtsractDataSet::swap(GtH5AbtsractDataSet& other) noexcept
{
//    qDebug() << "GtH5AbtsractDataSet::swap";
    using std::swap;
    swap(m_datatype, other.m_datatype);
    swap(m_dataspace, other.m_dataspace);
}
