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
    UnkownHook = 0, /// Invalid hook type

    PreDataSetReadHook,   /// Before reading from a dataset
    PostDataSetReadHook,  /// After reading from a dataset
    PreDataSetWriteHook,  /// Before writing to a dataset
    PostDataSetWriteHook, /// After writing to a dataset

    PreAttributeReadHook,   /// Before reading from an attribute
    PostAttributeReadHook,  /// After writing to an attribute
    PreAttributeWriteHook,  /// Before reading from an attribute
    PostAttributeWriteHook, /// After writing to an attribute

    /// Denotes the number of hooks, should not be used as a valid hook type
    NumberOfHooks = PostAttributeWriteHook
};

/// Enum indicating valid return values for hooks
enum HookReturnValue : int32_t
{
    HookContinue    = 0,  /// hook continues the execution (e.g. reading dataset)
    HookExitFailure = -1, /// hook exits calling function early with an error
    HookExitSuccess =  1  /// hook exits calling function early but no error is raised
};

/// Context parameter when executing a hook for writing to a dataset.
struct DataSetWriteHookContext
{
    /// Raw data that should be written to the dataset
    void const* data{};
    /// Dataspace denoting the layout of the data in the file (may be different
    /// from the dataset's dataspace, if a selection is active)
    DataSpace const* fileSpace{};
    /// Dataspace denoting the layout of `data`
    DataSpace const* memSpace{};
    /// Datatype denoting the type of data of `data` (may be different
    /// from the datatype of the dataset)
    DataType const* dataType{};
};

/// Context parameter when executing a hook for reading from a dataset.
struct DataSetReadHookContext
{
    /// Raw data that should be read into when reading from a dataset
    void* data{};
    /// Dataspace denoting the layout of the data in the file (may be different
    /// from the dataset's dataspace, if a selection is active)
    DataSpace const* fileSpace{};
    /// Dataspace denoting the layout of `data`
    DataSpace const* memSpace{};
    /// Datatype denoting the type of data of `data` (may be different
    /// from the datatype of the dataset)
    DataType const* dataType{};
};

/// Context parameter when executing a hook for writing to an attribute.
struct AttributeWriteHookContext
{
    /// Raw data that should be written to the attribute
    void const* data{};
    /// Datatype denoting the type of data of `data` (may be different
    /// from the datatype of the dataset)
    DataType const* dataType{};
};

/// Context parameter when executing a hook for reading from an attribute.
struct AttributeReadHookContext
{
    /// Raw data that should be read into when reading from an attribute
    void* data{};
    /// Datatype denoting the type of data of `data` (may be different
    /// from the datatype of the dataset)
    DataType const* dataType{};
};

/// Hook function signature.
///  - `id` belongs to the object the hook is executed on (e.g. `DataSet dset{id}`)
///  - `context` context object, can be safely casted to the corresponding hook
///     context type (never null).
using Hook = std::function<HookReturnValue(hid_t id, void* context)>;

/**
 * @brief Attempts to find the hook for the given `fileId`.
 * @throws InvalidArgumentError if `fileId` is not a valid file identifier or
 * if `type` is invalid.
 * @param fileId File identifier
 * @param type Hook type
 * @return Hook (may be null)
 */
GENH5_NODISCARD
GENH5_EXPORT
Hook findHook(hid_t fileId, HookType type) noexcept(false);

/**
 * @brief Unregisters the hook of type `type` from `fileId`.
 * @throws InvalidArgumentError if `fileId` is not a valid file identifier.
 * @param fileId File identifier
 * @param type Hook type
 * @return Success (if a hook was removed)
 */
GENH5_EXPORT
bool clearHook(hid_t fileId, HookType type);

/**
 * @brief Unregisters all hooks from `fileId`.
 * @throws InvalidArgumentError if `fileId` is not a valid file identifier.
 * @param fileId File identifier
 * @return Success (if hooks were registered)
 */
GENH5_EXPORT
bool clearHooks(hid_t fileId);

/**
 * @brief registerHook
 * @param fileId
 * @param type
 * @param hook
 */
GENH5_EXPORT
void registerHook(hid_t fileId, HookType type, Hook hook) noexcept(false);

template <typename File_t,
          traits::if_types_equal<File_t, File> = true> inline
void registerHook(File_t const& file, HookType type, Hook hook) noexcept(false)
{
    return registerHook(file.id(), type, std::move(hook));
}

/**
 * @brief Converts `hook` into a compatible functor. Useful if `hook` does not
 * return `HookReturnValue`. Defaults to `HookContinue`.
 * @param hook Hook to convert
 * @return Compatible hook functor
 */
template <typename T>
GENH5_NODISCARD
inline Hook makeHook(T hook)
{
    return [h = std::move(hook)](hid_t id, void* context){
        h(id, context);
        return HookContinue;
    };
}

} // namespace GenH5

#endif // GENH5_HOOKS_H
