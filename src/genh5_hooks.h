/* GenH5
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Author: Martin Siggel
 */

#ifndef GENH5_HOOKS_H
#define GENH5_HOOKS_H

#include <genh5_typedefs.h>
#include <genh5_exports.h>

#include <functional>

namespace GenH5
{

class DataSpace;
class DataType;
class File;

enum HookType : size_t
{
    UnkownHook = 0,

    PreDataSetRead,
    PostDataSetRead,
    PreDataSetWrite,
    PostDataSetWrite,

    PreAttributeRead,
    PostAttributeRead,
    PreAttributeWrite,
    PostAttributeWrite,

    NumberOfHooks = PostAttributeWrite
};

struct DataSetWriteHookContext
{
    void const* data{};
    DataSpace const* fileSpace{};
    DataSpace const* memSpace{};
    DataType const* dataType{};
};

struct DataSetReadHookContext
{
    void* data{};
    DataSpace const* fileSpace{};
    DataSpace const* memSpace{};
    DataType const* dataType{};
};

struct AttributeWriteHookContext
{
    void const* data{};
    DataType const* dataType{};
};

struct AttributeReadHookContext
{
    void* data{};
    DataType const* dataType{};
};

using Hook = std::function<void(hid_t id, void* context)>;

GENH5_EXPORT Hook findHook(hid_t fileId, HookType type);
GENH5_EXPORT Hook findHook(File const& file, HookType type);

GENH5_EXPORT void registerHook(File const& file, HookType type, Hook hook) noexcept(false);

} // namespace GenH5

#endif // GENH5_HOOKS_H
