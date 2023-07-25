/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.07.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#define GENH5_USE_QT_BINDINGS
#include "genh5_group.h"
#include "genh5_file.h"
#include "genh5_attribute.h"
#include "genh5_dataset.h"

#include "testhelper.h"

#include <QDebug>

/// This is a test fixture that does a init for each test
class TestH5Iteration : public testing::Test
{
protected:

    GenH5::Vector<GenH5::String> m_groups{
        "my_group_1",
        "my_group_1/sub1",
        "my_group_1/sub2",
        "my_group_2",
        "my_group_2/A",
        "my_group_2/B",
        "my_group_2/C",
        "my_group_2/C/final",
    };

    GenH5::Vector<GenH5::String> m_dsets{
        "my_fancy_dset",
        "my_group_1/dset1",
        "my_group_1/sub1/dsetA",
        "my_group_1/sub2/dsetB",
        "my_group_2/dset2",
        "my_group_2/C/final/final_dset",
    };

    int countChildNodes(GenH5::Vector<GenH5::String> const& list,
                        GenH5::Group const& parent)
    {
        if (parent.path().empty()) // relative to root
        {
            return GenH5::numeric_cast<int>(
                        std::count_if(list.cbegin(), list.cend(),
                              [](auto const& item){
                return !QByteArray::fromStdString(item).contains('/');
            }));
        }
        return -1;
    }

    void checkNodes(GenH5::Vector<GenH5::NodeInfo> const& nodes,
                    GenH5::Group const& parent)
    {
        qDebug().nospace()
                << "checking nodes relative to: '" << parent.path() << "'";
        auto all = concat(m_groups, m_dsets);
        for (auto const& info : nodes)
        {
            EXPECT_TRUE(info.isGroup() || info.isDataSet());
            EXPECT_TRUE(contains(all, info.path));
            EXPECT_TRUE(info.toNode(parent)->isValid());
        }
    }

    void checkGroups(GenH5::Vector<GenH5::NodeInfo> const& nodes,
                     GenH5::Group const& parent)
    {
        qDebug().nospace()
                << "checking groups relative to: '" << parent.path() << "'";
        for (auto const& info : nodes)
        {
            EXPECT_TRUE(info.isGroup());
            EXPECT_TRUE(contains(m_groups, info.path));
            EXPECT_TRUE(info.toGroup(parent).isValid());
            EXPECT_TRUE(info.toNode(parent)->isValid());
        }
    }

    void checkDsets(GenH5::Vector<GenH5::NodeInfo> const& nodes,
                    GenH5::Group const& parent)
    {
        qDebug().nospace()
                << "checking datasets relative to: '" << parent.path() << "'";
        for (auto const& info : nodes)
        {
            EXPECT_TRUE(info.isDataSet());
            EXPECT_TRUE(contains(m_dsets, info.path));
            EXPECT_TRUE(info.toDataSet(parent).isValid());
            EXPECT_TRUE(info.toNode(parent)->isValid());
        }
    }

    void createFileStructure(GenH5::Group const& root)
    {
        for (auto const& path : qAsConst(m_groups))
        {
            qDebug() << "creating group" << path;
            EXPECT_TRUE(root.createGroup(path).isValid());
        }
        for (auto const& path : qAsConst(m_dsets))
        {
            qDebug() << "creating dataset" << path;
            EXPECT_TRUE(root.createDataSet(path,
                                           GenH5::dataType<int>(),
                                           GenH5::DataSpace{2, 4}).isValid());
        }
    }
};

TEST_F(TestH5Iteration, index_order_enums)
{
    static_assert (GenH5::IterationIndex::IndexName ==
                   H5_INDEX_NAME, "Index types should match!");
    static_assert (GenH5::IterationIndex::IndexCreationOrder ==
                   H5_INDEX_CRT_ORDER, "Index types should match!");

    static_assert (GenH5::IterationOrder::NativeOrder ==
                   H5_ITER_NATIVE, "Order types should match!");
    static_assert (GenH5::IterationOrder::AscendingOrder ==
                   H5_ITER_INC, "Order types should match!");
    static_assert (GenH5::IterationOrder::DescendingOrder ==
                   H5_ITER_DEC, "Order types should match!");
}

TEST_F(TestH5Iteration, findChildObjects)
{
    GenH5::File file{h5TestHelper->newFilePath(), GenH5::Create};
    ASSERT_TRUE(file.isValid());

    qDebug() << "filepath" << file.filePath();

    auto root = file.root();

    EXPECT_EQ(root.findAttributes().size(), 0);
    EXPECT_EQ(root.findChildNodes().size(), 0);

    EXPECT_NO_THROW(root.writeVersionAttribute());
    EXPECT_EQ(root.findAttributes().size(), 1);
    EXPECT_EQ(root.findChildNodes().size(), 0);

    createFileStructure(root);

    // filter groups
    auto rootDGO = root.findChildNodes(GenH5::FindDirectOnly,
                                       GenH5::FilterGroups);
    EXPECT_EQ(rootDGO.size(), countChildNodes(m_groups, GenH5::Group{}));
    checkGroups(rootDGO, root);

    auto rootRGO = root.findChildNodes(GenH5::FindRecursive,
                                       GenH5::FilterGroups);
    EXPECT_EQ(rootRGO.size(), m_groups.size());
    checkGroups(rootRGO, root);

    // filter datasets
    auto rootDDO = root.findChildNodes(GenH5::FindDirectOnly,
                                       GenH5::FilterDataSets);
    EXPECT_EQ(rootDDO.size(), countChildNodes(m_dsets, GenH5::Group{}));
    checkDsets(rootDDO, root);

    auto rootRDO = root.findChildNodes(GenH5::FindRecursive,
                                       GenH5::FilterDataSets);
    EXPECT_EQ(rootRDO.size(), m_dsets.size());
    checkDsets(rootRDO, root);

    // all objects
    auto rootDAll = root.findChildNodes(GenH5::FindDirectOnly,
                                        GenH5::NoFilter);
    EXPECT_EQ(rootDAll.size(), countChildNodes(concat(m_groups, m_dsets), GenH5::Group{}));
    checkNodes(rootDAll, root);

    auto rootRAll = root.findChildNodes(GenH5::FindRecursive,
                                        GenH5::NoFilter);
    EXPECT_EQ(rootRAll.size(), m_dsets.size() + m_groups.size());
    checkNodes(rootRAll, root);
}

TEST_F(TestH5Iteration, sample)
{
    GenH5::File file{h5TestHelper->newFilePath(), GenH5::Create};

    // create a sample file structure
    auto a = file.root().createGroup("A");
    a.writeVersionAttribute();
    a.createAttribute("my_value", GenH5::dataType<double>(),
                      GenH5::DataSpace::Scalar);
    a.createDataSet("my_dset", GenH5::dataType<int>(),
                    GenH5::DataSpace::Scalar);

    auto data = a.createGroup("data");
    data.createDataSet("1", GenH5::dataType<int>(),
                       GenH5::DataSpace::linear(10)).
            writeVersionAttribute();
    data.createDataSet("2", GenH5::dataType<double>(),
                       GenH5::DataSpace{5, 10}).
            writeVersionAttribute();

    auto b = file.root().createGroup("B");
    b.writeVersionAttribute();
    b.createAttribute("my_attr", GenH5::dataType<QString>(),
                      GenH5::DataSpace::Scalar);
    b.createDataSet("empty", GenH5::dataType<char>(),
                    GenH5::DataSpace::Null);
    b.createGroup("my_sub_group");


    // iterate over all nodes
    auto allNodes = file.root().findChildNodes(GenH5::FindRecursive);
    EXPECT_EQ(allNodes.size(), 8);

    // find direct nodes only
    auto directNodes = file.root().findChildNodes(GenH5::FindDirectOnly);
    EXPECT_EQ(directNodes.size(), 2);

    // find all groups
    auto onlyGroups = file.root().findChildNodes(GenH5::FindRecursive,
                                                 GenH5::FilterGroups);
    EXPECT_EQ(onlyGroups.size(), 4);

    // find dsets relative to group A
    auto aDsets = a.findChildNodes(GenH5::FindRecursive,
                                   GenH5::FilterDataSets);
    EXPECT_EQ(aDsets.size(), 3);

    for (auto const& info : qAsConst(aDsets))
    {
        info.isDataSet(); // always true
        auto dset = info.toDataSet(a); // genuinely save
    }

    // find attributes
    auto bAttrs = b.findAttributes();
    EXPECT_EQ(bAttrs.size(), 2);

    b.iterateChildNodes([](GenH5::Group const& parent,
                           GenH5::NodeInfo const& info) -> herr_t {
        qDebug() << parent.path() + "/" + info.path;
        return 0;
    }, GenH5::FindRecursive);
}
