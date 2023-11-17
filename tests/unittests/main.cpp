/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QDebug>

#include "testhelper.h"

int main(int argc, char **argv)
{
    /*
    auto printIds = [](){
        auto const ids = GenH5::details::registeredIds();
        qDebug() << "Registered IDs:" << ids.size();
        for (auto id : ids)
        {
            auto const ref  = H5Iget_ref(id);
            auto const type = H5Iget_type(id);
            switch (type)
            {
            case H5I_FILE:
                qDebug() << id << "REF:" << ref << "FILE"; break;
            case H5I_DATASET:
                qDebug() << id << "REF:" << ref << "DSET"; break;
            case H5I_ATTR:
                qDebug() << id << "REF:" << ref << "ATRR"; break;
            case H5I_GROUP:
                qDebug() << id << "REF:" << ref << "GROUP"; break;
            case H5I_DATASPACE:
                qDebug() << id << "REF:" << ref << "SPACE"; break;
            case H5I_DATATYPE:
                qDebug() << id << "REF:" << ref << "DTYPE"; break;
            default:
                qDebug() << id << "REF:" << ref << "UNKOWN" << type; break;
            }
        }
    };
    */

    QCoreApplication app(argc, argv);

    auto* helper = TestHelper::instance();
    Q_UNUSED(helper)

    testing::InitGoogleTest(&argc, argv);

    int retval = RUN_ALL_TESTS();

    return retval;
}
