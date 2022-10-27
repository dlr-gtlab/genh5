/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
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
                                             GenH5::DataSpace::Scalar);

    int versionInt = GenH5::Version::current().toInt();
    EXPECT_TRUE(vattr.write(&versionInt));

    EXPECT_TRUE(file.root().hasVersionAttribute());

    // invalid format
    EXPECT_THROW(file.root().readVersionAttribute(), GenH5::AttributeException);
}
