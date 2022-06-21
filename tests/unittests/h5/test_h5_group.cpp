/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gth5_node.h"
#include "gth5_file.h"
#include "gth5_group.h"
#include "gth5_dataset.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Group : public testing::Test
{
protected:

    virtual void SetUp() override
    {
        file = GtH5::File(h5TestHelper->newFilePath(), GtH5::CreateOnly);
        ASSERT_TRUE(file.isValid());
    }

    GtH5::File file;
};

TEST_F(TestH5Group, isValid)
{
    GtH5::Group root;
    // test invalid dataset
    EXPECT_FALSE(root.isValid());

    // open the file as a root group
    root = GtH5::Group(file);
    EXPECT_TRUE(root.isValid());

    // create a first sub group
    GtH5::Group group = root.createGroup(QByteArrayLiteral("testA"));
    EXPECT_TRUE(group.isValid());

    // create another nested sub group
    GtH5::Group subGroup = group.createGroup(QByteArrayLiteral("testB"));
    EXPECT_TRUE(subGroup.isValid());
}
