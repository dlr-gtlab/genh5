/* GenH5
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Author: Marius Bröcker
 */

#include <genh5_hooks.h>
#include <genh5_file.h>
#include <genh5_exception.h>

#include <unordered_map>
#include <shared_mutex>

namespace HookDataBase
{

/// Hooks are organized as one big array per file id.
/// Each item in the array corresponds to the HookType of value "index+1".
using Entry = std::array<GenH5::Hook, GenH5::NumberOfHooks>;

using DataBase = std::unordered_map<hid_t, Entry>;

static DataBase& instance()
{
    static DataBase hooks;
    return hooks;
}

static std::shared_mutex& mutex()
{
    static std::shared_mutex mutex;
    return mutex;
}

} // namespace

GenH5::Hook
GenH5::findHook(hid_t fileId, HookType type) noexcept(false)
{
    if (!isValidId(fileId) && classType(fileId) == IdType::File)
    {
        throw IdComponentException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid file id"
        };
    }
    if (type == UnkownHook || type > NumberOfHooks)
    {
        throw IdComponentException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid hook type"
        };
    }

    auto& mutex = HookDataBase::mutex();
    std::shared_lock<std::shared_mutex> lock(mutex);

    auto& dataBase = HookDataBase::instance();
    auto iter = dataBase.find(fileId);
    if (iter == dataBase.end()) return {};

    return (iter->second)[type - 1];
}

bool
GenH5::clearHook(hid_t fileId, HookType type)
{
    if (!isValidId(fileId) && classType(fileId) == IdType::File)
    {
        throw IdComponentException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid file id"
        };
    }
    if (type > NumberOfHooks)
    {
        throw IdComponentException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid hook type"
        };
    }

    auto& mutex = HookDataBase::mutex();
    std::unique_lock<std::shared_mutex> lock(mutex);

    auto& dataBase = HookDataBase::instance();
    auto iter = dataBase.find(fileId);
    if (iter == dataBase.end()) return false;

    // remove all hooks
    if (type == UnkownHook) dataBase.erase(iter);
    // remove only the selected hook
    else iter->second[type - 1] = {};

    return true;
}

bool
GenH5::clearHooks(hid_t fileId)
{
    return clearHook(fileId, UnkownHook);
}

void
GenH5::registerHook(hid_t fileId, HookType type, Hook hook) noexcept(false)
{
    if (!isValidId(fileId) && classType(fileId) == IdType::File)
    {
        throw IdComponentException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid file id"
        };
    }
    if (type == UnkownHook || type > NumberOfHooks)
    {
        throw IdComponentException{
            GENH5_MAKE_EXECEPTION_STR() "Invalid hook type"
        };
    }

    auto& mutex = HookDataBase::mutex();
    std::unique_lock<std::shared_mutex> lock(mutex);

    auto& dataBase = HookDataBase::instance();
    auto iter = dataBase.find(fileId);
    if (iter == dataBase.end())
    {
        iter = dataBase.insert({ fileId, HookDataBase::Entry{} }).first;
    }

    (iter->second)[type - 1] = std::move(hook);
}
