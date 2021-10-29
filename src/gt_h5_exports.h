/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GT_H5_EXPORTS_H
#define GT_H5_EXPORTS_H

#if defined(WIN32)
  #if defined (GT_H5_DLL)
    #define GT_H5_EXPORT __declspec (dllexport)
  #else
    #define GT_H5_EXPORT __declspec (dllimport)
  #endif
#else
    #define GT_H5_EXPORT
#endif

#endif // GT_H5_EXPORTS_H
