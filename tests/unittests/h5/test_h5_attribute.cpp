/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "genh5_attribute.h"
#include "genh5_file.h"

#include "testhelper.h"

/// This is a test fixture that does a init for each test
class TestH5Attribute : public testing::Test { };

TEST_F(TestH5Attribute, deleteLink)
{
    GenH5::Attribute attr;
    // test invalid attribute
    EXPECT_FALSE(attr.isValid());

    auto file = GenH5::File(h5TestHelper->newFilePath(), GenH5::Create);
    // create valid attribute
    attr = file.root().createAttribute(QByteArrayLiteral("test"),
                                       GenH5::dataType<int>(),
                                       GenH5::DataSpace::linear(10));
    EXPECT_TRUE(attr.isValid());
    EXPECT_TRUE(file.root().hasAttribute(QByteArrayLiteral("test")));

    // delete dataset
    attr.deleteLink();
    EXPECT_FALSE(attr.isValid());
    EXPECT_FALSE(file.root().hasAttribute(QByteArrayLiteral("test")));
}
