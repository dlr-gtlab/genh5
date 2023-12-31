/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 05.07.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_EXPORTS_H
#define GENH5_EXPORTS_H

#if GENH5_STATIC
  #define GENH5_EXPORT
#else
  #if defined(WIN32)
    #if defined (GENH5_DLL)
      #define GENH5_EXPORT __declspec (dllexport)
    #else
      #define GENH5_EXPORT __declspec (dllimport)
    #endif
  #else
    #define GENH5_EXPORT
  #endif
#endif

#endif // GENH5_EXPORTS_H
