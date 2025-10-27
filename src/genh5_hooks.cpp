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

namespace HookDataBase
{

using Entry = std::array<GenH5::Hook, GenH5::NumberOfHooks>;

using DataBase = std::unordered_map<hid_t, Entry>;

static DataBase& instance()
{
    static DataBase hooks;
    return hooks;
}

} // namespace

GenH5::Hook
GenH5::findHook(hid_t fileId, HookType type)
{
    if (!isValidId(fileId))
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

    auto& dataBase = HookDataBase::instance();
    auto iter = dataBase.find(fileId);
    if (iter == dataBase.end()) return {};

    return (iter->second)[type - 1];
}

GenH5::Hook
GenH5::findHook(File const& file, HookType type)
{
    return findHook(file.id(), type);
}

void
GenH5::registerHook(File const& file, HookType type, Hook hook) noexcept(false)
{
    if (!file.isValid())
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

    auto& dataBase = HookDataBase::instance();
    auto iter = dataBase.find(file.id());
    if (iter == dataBase.end())
    {
        iter = dataBase.insert({ file.id(), HookDataBase::Entry{} }).first;
    }

    (iter->second)[type - 1] = std::move(hook);
}
