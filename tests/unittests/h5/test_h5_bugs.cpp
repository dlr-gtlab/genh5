/* GenH5
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#include "genh5_file.h"

#include "gtest/gtest.h"

TEST(Bugs, GroupNameTruncation)
{
    auto file = GenH5::File("truncation.h5", GenH5::Overwrite);
    auto group = file.root().createGroup("Root5678901234567890123456");
    auto testgroup = group.createGroup("test"); // all together 32 byte including 2 "/"

    QString groupName = testgroup.name();

    EXPECT_EQ("test", groupName.toStdString());
}
