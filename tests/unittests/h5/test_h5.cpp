/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 29.04.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "genh5_version.h"

#include "gtest/gtest.h"
#include "H5public.h"

#include <QDebug>

/// This is a test fixture that does a init for each test
class TestH5 : public testing::Test {};

TEST_F(TestH5, verionMacros)
{
    // should compile
    qDebug() << "version hex:" << QString::number(GENH5_VERSION, 16);
    qDebug() << "version str:" << QStringLiteral(GENH5_VERSION_STR);
    qDebug() << "major:   " << int{GENH5_VERSION_MAJOR};
    qDebug() << "minor:   " << int{GENH5_VERSION_MINOR};
    qDebug() << "patch:   " << int{GENH5_VERSION_PATCH};
    qDebug() << "addition:" << QStringLiteral(GENH5_VERSION_ADDITIONAL);

    auto v = GenH5::Version::current();
    qDebug().nospace() << "Version::current() = "
                       << v.major << "-" << v.minor << "-" << v.patch;

    EXPECT_TRUE(GENH5_VERSION == GenH5::Version::current());
}

TEST_F(TestH5, hdf5Version)
{
    EXPECT_TRUE(0x010c00 < GenH5::Version::hdf5());
    EXPECT_TRUE(0x010d00 > GenH5::Version::hdf5());
}

TEST_F(TestH5, verionCheck)
{
    EXPECT_TRUE(GENH5_VCHECK(1, 1, 1) > 0x010100);
    EXPECT_TRUE(GENH5_VCHECK(1, 1, 1) != 0x010100);
    EXPECT_TRUE(GENH5_VCHECK(1, 1, 1) == 0x010101);
    EXPECT_TRUE(GENH5_VCHECK(1, 1, 1) >= 0x010101);
    EXPECT_TRUE(GENH5_VCHECK(1, 1, 1) <= 0x010101);
    EXPECT_TRUE(GENH5_VCHECK(2, 0, 1) < 0x020101);
    EXPECT_TRUE(GENH5_VCHECK(1, 2, 1) > 0x000101);
}

TEST_F(TestH5, operators)
{
    EXPECT_TRUE((GenH5::Version{1, 1, 1}  > GenH5::Version{0x010100}));
    EXPECT_TRUE((GenH5::Version{1, 1, 1} != GenH5::Version{0x010100}));
    EXPECT_TRUE((GenH5::Version{1, 1, 1} == GenH5::Version{0x010101}));
    EXPECT_TRUE((GenH5::Version{1, 1, 1} <= GenH5::Version{0x010101}));
    EXPECT_TRUE((GenH5::Version{1, 1, 1} >= GenH5::Version{0x010101}));
    EXPECT_TRUE((GenH5::Version{2, 0, 1}  < GenH5::Version{0x020103}));
    EXPECT_TRUE((GenH5::Version{1, 2, 1}  > GenH5::Version{0x000101}));
}
