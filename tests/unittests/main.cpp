/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QScopedPointer>

#include "testhelper.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    auto* helper = TestHelper::instance();
    Q_UNUSED(helper)

    testing::InitGoogleTest(&argc, argv);

    int retval = RUN_ALL_TESTS();

    return retval;
}
