/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 *  Created on: 1.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_file.h"
#include "genh5_group.h"
#include "genh5_attribute.h"

#include "testhelper.h"

class TestH5Node : public testing::Test { };

TEST_F(TestH5Node, versionAttribute)
{
    ASSERT_FALSE(GenH5::Node::versionAttributeName().isEmpty());

    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);

    ASSERT_NO_THROW(file.root().writeVersionAttribute());

    EXPECT_TRUE(file.root().hasVersionAttribute());

    auto version = file.root().readVersionAttribute();
    auto current = GenH5::Version::current();
    EXPECT_EQ(version.major, current.major);
    EXPECT_EQ(version.minor, current.minor);
    EXPECT_EQ(version.patch, current.patch);
}

TEST_F(TestH5Node, versionAttributeInvalid)
{
    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);

    EXPECT_FALSE(file.root().hasVersionAttribute());

    auto vattr = file.root().createAttribute(GenH5::Node::versionAttributeName(),
                                             GenH5::dataType<int>(),
                                             GenH5::DataSpace::Scalar());

    int versionInt = GenH5::Version::current().toInt();
    EXPECT_TRUE(vattr.write(&versionInt));

    EXPECT_TRUE(file.root().hasVersionAttribute());

    // invalid format
    EXPECT_THROW(file.root().readVersionAttribute(), GenH5::AttributeException);
}

TEST_F(TestH5Node, parent)
{
    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);

    /**
     * Group
     */

    EXPECT_FALSE(file.root().hasParent());
    EXPECT_THROW(file.root().parent(), GenH5::GroupException);

    EXPECT_FALSE(GenH5::Group{}.hasParent());
    EXPECT_THROW(GenH5::Group{}.parent(), GenH5::GroupException);

    GenH5::Group a = file.root().createGroup("a");
    GenH5::Group b = a.createGroup("b");

    EXPECT_TRUE(a.hasParent());
    EXPECT_TRUE(a.parent().isValid());
    EXPECT_EQ(a.parent().name(), "/");
    EXPECT_TRUE(b.hasParent());
    EXPECT_TRUE(b.parent().isValid());
    EXPECT_EQ(b.parent().name(), "a");

    /**
     * Dataset
     */

    EXPECT_FALSE(GenH5::DataSet{}.hasParent());
    EXPECT_THROW(GenH5::DataSet{}.parent(), GenH5::GroupException);

    GenH5::DataSet dsetA = a.writeDataSet0D("test", 42);
    GenH5::DataSet dsetRoot = file.root().writeDataSet0D("testRoot", 42);

    EXPECT_TRUE(dsetA.hasParent());
    EXPECT_TRUE(dsetA.parent().isValid());
    EXPECT_EQ(dsetA.parent().name(), "a");

    EXPECT_TRUE(dsetRoot.hasParent());
    EXPECT_TRUE(dsetRoot.parent().isValid());
    EXPECT_EQ(dsetRoot.parent().name(), "/");
}

