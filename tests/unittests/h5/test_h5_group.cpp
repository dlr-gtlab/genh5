/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 22.09.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"
#include "gt_h5node.h"
#include "gt_h5file.h"
#include "gt_h5group.h"

#include "testhelper.h"


/// This is a test fixture that does a init for each test
class TestH5Group : public testing::Test
{
protected:

    virtual void SetUp()
    {
        file = GtH5File(TestHelper::instance()->newFilePath(),
                        GtH5File::CreateOverwrite);
        ASSERT_TRUE(file.isValid());
    }

    GtH5File file;
};

TEST_F(TestH5Group, isValid)
{
    GtH5Group root;
    // test invalid dataset
    EXPECT_FALSE(root.isValid());

    // open the file as a root group
    root = file;
    EXPECT_TRUE(root.isValid());

    // create a first sub group
    GtH5Group group = root.createGroup(QByteArrayLiteral("testA"));
    EXPECT_TRUE(group.isValid());

    // create another nested sub group
    GtH5Group subGroup = group.createGroup(QByteArrayLiteral("testB"));
    EXPECT_TRUE(subGroup.isValid());
}
