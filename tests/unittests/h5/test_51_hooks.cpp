/* GenH5
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Author: Marius Bröcker
 */

#include "gtest/gtest.h"

#include <genh5_hooks.h>
#include <genh5_attribute.h>
#include <genh5_dataset.h>
#include <genh5_dataspace.h>
#include <genh5_datatype.h>
#include <genh5_file.h>

#include "testhelper.h"

TEST(Test_51_Hooks, register_and_unregister_hook)
{
    GenH5::String filePath = h5TestHelper->newFilePath(test_info_);
    GenH5::File file{filePath, GenH5::Create};

    GenH5::registerHook(file, GenH5::PreDataSetWriteHook, GenH5::makeHook([](auto...){ }));
    GenH5::registerHook(file, GenH5::PostDataSetWriteHook, GenH5::makeHook([](auto...){ }));

    // only registered hooks are available
    EXPECT_TRUE(GenH5::findHook(file, GenH5::PreDataSetWriteHook));
    EXPECT_TRUE(GenH5::findHook(file, GenH5::PostDataSetWriteHook));
    EXPECT_FALSE(GenH5::findHook(file, GenH5::PreDataSetReadHook));
    EXPECT_FALSE(GenH5::findHook(file, GenH5::PostDataSetReadHook));

    GenH5::clearHook(file, GenH5::PreDataSetWriteHook);

    // only desired hook deleted
    EXPECT_FALSE(GenH5::findHook(file, GenH5::PreDataSetWriteHook));
    EXPECT_TRUE(GenH5::findHook(file, GenH5::PostDataSetWriteHook));

    ASSERT_EQ(GenH5::refCount(file.id()), 1);

    file.close();

    // all hooks deleted
    EXPECT_THROW((void)GenH5::findHook(file, GenH5::PreDataSetWriteHook), GenH5::IdComponentException);
    EXPECT_THROW((void)GenH5::findHook(file, GenH5::PostDataSetWriteHook), GenH5::IdComponentException);
}

TEST(Test_51_Hooks, number_of_hooks)
{
    auto hooks = {
        GenH5::PreDataSetWriteHook,
        GenH5::PostDataSetWriteHook,
        GenH5::PreDataSetReadHook,
        GenH5::PostDataSetReadHook,
        GenH5::PreAttributeWriteHook,
        GenH5::PostAttributeWriteHook,
        GenH5::PreAttributeReadHook,
        GenH5::PostAttributeReadHook
    };

    size_t max_value = std::max(hooks);

    ASSERT_EQ(max_value, (GenH5::NumberOfHooks - 1));

    GenH5::String filePath = h5TestHelper->newFilePath(test_info_);
    GenH5::File file{filePath, GenH5::Create};

    for (GenH5::HookType type : hooks)
    {
        GenH5::registerHook(file, type, GenH5::makeHook([](auto...){ }));
        EXPECT_TRUE(GenH5::findHook(file, type));
    }

    for (GenH5::HookType type : hooks)
    {
        EXPECT_TRUE(GenH5::clearHook(file, type));
        EXPECT_FALSE(GenH5::findHook(file, type));
    }

    EXPECT_THROW(GenH5::registerHook(file,
                                     GenH5::NumberOfHooks,
                                     GenH5::makeHook([](auto...){ })),
                 GenH5::IdComponentException);
    EXPECT_THROW((void)GenH5::findHook(file, GenH5::NumberOfHooks),
                 GenH5::IdComponentException);
    EXPECT_THROW(GenH5::clearHook(file, GenH5::NumberOfHooks),
                 GenH5::IdComponentException);
}

TEST(Test_51_Hooks, execute_hook)
{
    GenH5::String filePath = h5TestHelper->newFilePath(test_info_);
    GenH5::File file{filePath, GenH5::Create};

    qDebug() << "registering hooks for file" << file.id();

    bool preHookExecuted = false;
    bool postHookExecuted = false;
    GenH5::String dsetPath;
    void const* dataPtr = nullptr;

    auto preWrite = [&](hid_t id, void* contextObject){
        preHookExecuted = true;

        auto* context = static_cast<GenH5::DataSetWriteHookContext*>(contextObject);
        dataPtr = context->data;

        GenH5::DataSet dset{id};
        EXPECT_TRUE(dset.isValid());
        dsetPath = dset.path();
        return GenH5::HookContinue;
    };

    auto postWrite = [&](hid_t id, void* contextObject){
        postHookExecuted = true;

        auto* context = static_cast<GenH5::DataSetWriteHookContext*>(contextObject);
        dataPtr = context->data;

        GenH5::DataSet dset{id};
        EXPECT_TRUE(dset.isValid());
        dsetPath = dset.path();
        return GenH5::HookContinue;
    };

    GenH5::registerHook(file, GenH5::HookType::PreDataSetWriteHook, preWrite);
    GenH5::registerHook(file, GenH5::HookType::PostDataSetWriteHook, postWrite);

    GenH5::Data<uint64_t> data;
    data.resize(100);
    std::iota(data.begin(), data.end(), 0);

    GenH5::DataSet dset = file.root().writeDataSet("test_dset", data);

    EXPECT_TRUE(preHookExecuted);
    EXPECT_TRUE(postHookExecuted);
    EXPECT_EQ(dsetPath, dset.path());
    EXPECT_EQ(dataPtr, data.data());

    preHookExecuted = false;
    postHookExecuted = false;
    dsetPath = {};
    dataPtr = nullptr;

    qDebug() << "reopening file...";

    // file id is different -> hooks wont be executed
    hid_t oldId = file.id();
    file = GenH5::File{filePath, GenH5::ReadWrite};

    ASSERT_NE(oldId, file.id());

    std::iota(data.begin(), data.end(), 100);
    file.root().writeDataSet("test_dset", data);

    EXPECT_FALSE(preHookExecuted);
    EXPECT_FALSE(postHookExecuted);
    EXPECT_NE(dsetPath, dset.path());
    EXPECT_NE(dataPtr, data.data());
}

TEST(Test_51_Hooks, execute_hook_closed_file)
{
    GenH5::String filePath = h5TestHelper->newFilePath(test_info_);

    bool preHookExecuted = false;
    bool postHookExecuted = false;

    GenH5::DataSet dset;

    GenH5::Data<uint64_t> data;
    data.resize(100);
    std::iota(data.begin(), data.end(), 0);

    hid_t fileId{};

    // hooks are only active as long as the main file handle is alive
    {
        GenH5::File file{filePath, GenH5::Create};
        fileId = file.id();
        qDebug() << "registering hooks for file" << file.id();

        auto preWrite = [&](hid_t id, void* contextObject){
            preHookExecuted = true;
            return GenH5::HookContinue;
        };

        auto postWrite = [&](hid_t id, void* contextObject){
            postHookExecuted = true;
            return GenH5::HookContinue;
        };

        GenH5::registerHook(file, GenH5::PreDataSetWriteHook, preWrite);
        GenH5::registerHook(file, GenH5::PostDataSetWriteHook, postWrite);

        // only registered hooks are available
        EXPECT_TRUE(GenH5::findHook(file, GenH5::PreDataSetWriteHook));
        EXPECT_TRUE(GenH5::findHook(file, GenH5::PostDataSetWriteHook));

        dset = file.root().writeDataSet("test_dset", data);
        EXPECT_TRUE(dset.isValid());
        EXPECT_TRUE(dset.dataSpace() == data.dataSpace());

        EXPECT_TRUE(preHookExecuted);
        EXPECT_TRUE(postHookExecuted);

        preHookExecuted  = false;
        postHookExecuted = false;
    }

    // dset is still open
    EXPECT_TRUE(dset.isValid());
    EXPECT_TRUE(GenH5::isValidId(dset.fileId()));

    // hooks are now deleted, since main handle is closed
    EXPECT_NE(dset.fileId(), fileId);
    EXPECT_FALSE(GenH5::isValidId(fileId));
    EXPECT_FALSE(GenH5::findHook(dset.file(), GenH5::PreDataSetWriteHook));
    EXPECT_FALSE(GenH5::findHook(dset.file(), GenH5::PostDataSetWriteHook));

    // write new data to check if hooks are executed
    for (auto& element : data) element *= 2;
    ASSERT_TRUE(dset.write(data));
    EXPECT_TRUE(dset.dataSpace() == data.dataSpace());

    EXPECT_FALSE(preHookExecuted);
    EXPECT_FALSE(postHookExecuted);
}

TEST(Test_51_Hooks, hook_return_value)
{
    GenH5::String filePath = h5TestHelper->newFilePath(test_info_);
    GenH5::File file{filePath, GenH5::Create};

    bool preHookExecuted = false;
    bool postHookExecuted = false;

    GenH5::HookReturnValue preHookReturnValue = GenH5::HookExitSuccess;
    GenH5::HookReturnValue postHookReturnValue = GenH5::HookExitFailure;

    auto preWrite = [&](hid_t id, void* contextObject){
        preHookExecuted = true;
        return preHookReturnValue;
    };

    auto postWrite = [&](hid_t id, void* contextObject){
        postHookExecuted = true;
        return postHookReturnValue;
    };

    GenH5::registerHook(file, GenH5::HookType::PreDataSetWriteHook, preWrite);
    GenH5::registerHook(file, GenH5::HookType::PostDataSetWriteHook, postWrite);

    GenH5::Data<uint64_t> data;
    data.resize(100);
    std::iota(data.begin(), data.end(), 0);

    // pre hook exists with success -> no write happened
    EXPECT_NO_THROW(file.root().writeDataSet("test_dset_1", data));

    EXPECT_TRUE(preHookExecuted);
    EXPECT_FALSE(postHookExecuted);

    // pre hook exists with failure -> helper function throws
    preHookReturnValue = GenH5::HookExitFailure;
    preHookExecuted = false;
    postHookExecuted = false;

    EXPECT_THROW(file.root().writeDataSet("test_dset_2", data),
                 GenH5::DataSetException);

    EXPECT_TRUE(preHookExecuted);
    EXPECT_FALSE(postHookExecuted);

    // pre hook continue like normal -> post hook fails -> helper function throws
    preHookReturnValue = GenH5::HookContinue;
    preHookExecuted = false;
    postHookExecuted = false;

    EXPECT_THROW(file.root().writeDataSet("test_dset_3", data),
                 GenH5::DataSetException);

    EXPECT_TRUE(preHookExecuted);
    EXPECT_TRUE(postHookExecuted);

    // pre and post hook continue like normal -> no throw
    postHookReturnValue = GenH5::HookContinue;
    preHookExecuted = false;
    postHookExecuted = false;

    EXPECT_NO_THROW(file.root().writeDataSet("test_dset_3", data));

    EXPECT_TRUE(preHookExecuted);
    EXPECT_TRUE(postHookExecuted);
}
