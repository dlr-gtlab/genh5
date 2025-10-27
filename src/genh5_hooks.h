/* GenH5
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Author: Martin Siggel
 */

#ifndef GENH5_HOOKS_H
#define GENH5_HOOKS_H

#include <genh5_globals.h>
#include <genh5_typedefs.h>
#include <genh5_typetraits.h>
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

    PreDataSetReadHook,
    PostDataSetReadHook,
    PreDataSetWriteHook,
    PostDataSetWriteHook,

    PreAttributeReadHook,
    PostAttributeReadHook,
    PreAttributeWriteHook,
    PostAttributeWriteHook,

    NumberOfHooks = PostAttributeWriteHook
};

enum HookReturnValue
{
    HookContinue = 0,
    HookExitFailure,
    HookExitSuccess
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

using Hook = std::function<HookReturnValue(hid_t id, void* context)>;

GENH5_NODISCARD
GENH5_EXPORT
Hook findHook(hid_t fileId, HookType type) noexcept(false);

GENH5_EXPORT
bool clearHook(hid_t fileId, HookType type);
GENH5_EXPORT
bool clearHooks(hid_t fileId);

GENH5_EXPORT
void registerHook(hid_t fileId, HookType type, Hook hook) noexcept(false);

template <typename File_t,
          traits::if_types_equal<File_t, File> = true>
inline void
registerHook(File_t const& file, HookType type, Hook hook) noexcept(false)
{
    return registerHook(file.id(), type, std::move(hook));
}

} // namespace GenH5

#endif // GENH5_HOOKS_H
