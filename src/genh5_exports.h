/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 05.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_EXPORTS_H
#define GENH5_EXPORTS_H

#if defined(WIN32)
  #if defined (GENH5_DLL)
    #define GENH5_EXPORT __declspec (dllexport)
  #else
    #define GENH5_EXPORT __declspec (dllimport)
  #endif
#else
    #define GENH5_EXPORT
#endif

#endif // GENH5_EXPORTS_H
