/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "testhelper.h"

#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    auto* helper = TestHelper::instance();
    Q_UNUSED(helper)

    testing::InitGoogleTest(&argc, argv);

    int retval = RUN_ALL_TESTS();

    return retval;
}
