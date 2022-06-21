/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 19.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_CONVERSION_DEFAULTS_H
#define GTH5_CONVERSION_DEFAULTS_H

#include "gth5_datatype.h"
#include "buffer.h"

/**
  * Checklist for supporting new types
  * 1. Register Conversion Type (either using implicit or explicit conversion)
  *    a) implicit: double -> double
  *    b) explciit QString -> char*
  * 3. Register HDF5 Data Type
  * 4. Register a Buffer Type if conversion requires buffering
  * 5. Register a Conversion Method if using explicit conversion
  * 6. Register a Reverse Conversion if implicit reverse conversion does not
  *    work
  */

#define GTH5_DECLARE_IMPLICIT_CONVERSION(TYPE) \
    /* buffered */ \
    inline TYPE convert(TYPE value, GtH5::buffer_t<TYPE>& /*...*/) { \
        return value; \
    } \
    /* plain */ \
    inline TYPE convert(TYPE value) { \
        return value; \
    } \

#define GTH5_DECLARE_CONVERSION(TYPE_FROM, VALUE_NAME, BUFFER_NAME) \
    inline GtH5::conversion_t<std::decay_t<TYPE_FROM>> \
    convert(TYPE_FROM VALUE_NAME,  \
            GtH5::buffer_t<std::decay_t<TYPE_FROM>>& BUFFER_NAME)

#define GTH5_DECLARE_REVERSE_CONVERSION(TYPE_TO, TYPE_FROM, VALUE_NAME) \
    template <> \
    inline TYPE_TO \
    GtH5::convertTo<TYPE_TO,TYPE_FROM>(TYPE_FROM VALUE_NAME)

namespace GtH5
{

/* GENERIC REVERSE CONVERSION */
template<typename Ttarget, typename Tfrom = Ttarget&&>
inline Ttarget
convertTo(Tfrom value)
{
    // generic reverse conversion
    return Ttarget{std::forward<Tfrom>(value)};
}

// default conversions
GTH5_DECLARE_IMPLICIT_CONVERSION(bool);

GTH5_DECLARE_IMPLICIT_CONVERSION(char);
GTH5_DECLARE_IMPLICIT_CONVERSION(char*);
GTH5_DECLARE_IMPLICIT_CONVERSION(char const*);

GTH5_DECLARE_IMPLICIT_CONVERSION(int);
GTH5_DECLARE_IMPLICIT_CONVERSION(long int);
GTH5_DECLARE_IMPLICIT_CONVERSION(long long int);
GTH5_DECLARE_IMPLICIT_CONVERSION(unsigned int);
GTH5_DECLARE_IMPLICIT_CONVERSION(unsigned long int);
GTH5_DECLARE_IMPLICIT_CONVERSION(unsigned long long int);

GTH5_DECLARE_IMPLICIT_CONVERSION(float);
GTH5_DECLARE_IMPLICIT_CONVERSION(double);

} // namespace GtH5

#endif // GTH5_CONVERSION_DEFAULTS_H
