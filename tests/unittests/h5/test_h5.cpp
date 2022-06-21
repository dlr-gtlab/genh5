
#include "gtest/gtest.h"

#include "gth5_version.h"
#include "gth5_object.h"

#include <QDebug>

/// This is a test fixture that does a init for each test
class TestH5 : public testing::Test {};

TEST_F(TestH5, verion)
{
    // should compile
    qDebug() << "version hex:" << QString::number(GTH5_VERSION, 16);
    qDebug() << "version str:" << QStringLiteral(GTH5_VERSION_STR);
    qDebug() << "major:   " << int{GTH5_VERSION_MAJOR};
    qDebug() << "minor:   " << int{GTH5_VERSION_MINOR};
    qDebug() << "patch:   " << int{GTH5_VERSION_PATCH};
    qDebug() << "addition:" << QStringLiteral(GTH5_VERSION_ADDITIONAL);
}
