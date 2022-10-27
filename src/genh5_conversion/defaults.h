/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 19.05.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_CONVERSION_DEFAULTS_H
#define GENH5_CONVERSION_DEFAULTS_H

#include "genh5_datatype.h"
#include "genh5_conversion/buffer.h"

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

#define GENH5_DECLARE_IMPLICIT_CONVERSION(TYPE) \
    /* buffered */ \
    inline GenH5::conversion_t<GenH5::traits::decay_crv_t<TYPE>> \
    convert(TYPE value, GenH5::buffer_t<TYPE>& /*...*/) { \
        return value; \
    } \
    /* plain */ \
    inline GenH5::conversion_t<GenH5::traits::decay_crv_t<TYPE>> \
    convert(TYPE value) { \
        return value; \
    } \

#define GENH5_DECLARE_CONVERSION(TYPE_FROM, VALUE_NAME, BUFFER_NAME) \
    inline GenH5::conversion_t<GenH5::traits::decay_crv_t<TYPE_FROM>> \
    convert(TYPE_FROM VALUE_NAME,  \
            GenH5::buffer_t<GenH5::traits::decay_crv_t<TYPE_FROM>>& BUFFER_NAME)

#define GENH5_DECLARE_REVERSE_CONVERSION(TYPE_TO, TYPE_FROM, VALUE_NAME) \
    template <> \
    inline auto \
    GenH5::convertTo<TYPE_TO,TYPE_FROM>(TYPE_FROM VALUE_NAME)

namespace GenH5
{

/** GENERIC REVERSE CONVERSION **/
template<typename Ttarget, typename Tfrom = Ttarget&&>
inline auto
convertTo(Tfrom value)
{
    // generic reverse conversion
    return static_cast<Ttarget>(std::forward<Tfrom>(value));
}

// default conversions
GENH5_DECLARE_IMPLICIT_CONVERSION(bool);

GENH5_DECLARE_IMPLICIT_CONVERSION(char);
GENH5_DECLARE_IMPLICIT_CONVERSION(char*);
GENH5_DECLARE_IMPLICIT_CONVERSION(char const*);

GENH5_DECLARE_IMPLICIT_CONVERSION(int);
GENH5_DECLARE_IMPLICIT_CONVERSION(long int);
GENH5_DECLARE_IMPLICIT_CONVERSION(long long int);
GENH5_DECLARE_IMPLICIT_CONVERSION(unsigned int);
GENH5_DECLARE_IMPLICIT_CONVERSION(unsigned long int);
GENH5_DECLARE_IMPLICIT_CONVERSION(unsigned long long int);

GENH5_DECLARE_IMPLICIT_CONVERSION(float);
GENH5_DECLARE_IMPLICIT_CONVERSION(double);

GENH5_DECLARE_IMPLICIT_CONVERSION(Version);

} // namespace GenH5

#endif // GENH5_CONVERSION_DEFAULTS_H
