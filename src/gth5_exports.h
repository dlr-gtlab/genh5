/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_EXPORTS_H
#define GTH5_EXPORTS_H

#if defined(WIN32)
  #if defined (GTH5_DLL)
    #define GTH5_EXPORT __declspec (dllexport)
  #else
    #define GTH5_EXPORT __declspec (dllimport)
  #endif
#else
    #define GTH5_EXPORT
#endif

#endif // GTH5_EXPORTS_H
