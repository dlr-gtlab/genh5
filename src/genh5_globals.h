/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 *  Created on: 30.1.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GENH5_GLOBALS_H
#define GENH5_GLOBALS_H

#define GENH5_TO_STR_HELPER(x) #x
#define GENH5_TO_STR(x) GENH5_TO_STR_HELPER(x)

#if defined(__has_cpp_attribute)
  #if __has_cpp_attribute(nodiscard)
    #define GENH5_NODISCARD [[nodiscard]]
  #else
    #define GENH5_NODISCARD
  #endif
#else
  #define GENH5_NODISCARD
#endif

#endif // GENH5_GLOBALS_H
