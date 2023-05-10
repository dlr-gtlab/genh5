
#include "gtest/gtest.h"
#include "testhelper.h"

#include <QDebug>
#include <QPoint>
#include <QPointF>

#include "genh5.h"

#include "H5Cpp.h"

struct Tests : public testing::Test
{
    GenH5::File makeFile(const char* name)
    {
        h5TestHelper->newFilePath();
        auto filePath = h5TestHelper->tempPath().toStdString() + "/" + name;
        return GenH5::File{filePath, GenH5::Overwrite};
    }
};

TEST_F(Tests, test_1)
{
    using GenH5::Version;
    using GenH5::Array;
    using GenH5::VarLen;
    using GenH5::Comp;

    auto file = makeFile("datatype_test.h5");

    // pod
    auto dset = file.root().writeDataSet0D("bool", true);



    auto scalar = file.root().createGroup("Scalar");

    //strings
    scalar.writeDataSet("string_fixed", GenH5::makeFixedStr("Hello World"));
    scalar.writeDataSet0D("string_varlen", QString("This is a very basic string! ☺"));

    // compound
    scalar.writeDataSet0D("compound_version", Version::current());

    GenH5::CompData0D<int, Version> nestedCompData{
        42, Version::current()
    };
    scalar.writeDataSet("compound_nested", nestedCompData);

    // varlen
    scalar.writeDataSet0D("varlen_double", VarLen<double>{42.1, 13.2, 33});
    scalar.writeDataSet0D("varlen_string", VarLen<QString>{"42.1", "13.2", "33"});



    auto linear = file.root().createGroup("Linear");

    // ints
    linear.writeDataSet("ints", h5TestHelper->linearDataVector(60));

    // array
    GenH5::Data<Array<int, 3>> arrayData{
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    linear.writeDataSet("array_3xint", arrayData);

    // complex
    GenH5::CompData<VarLen<Array<double, 3>>> complexVlenArrayData{
        VarLen<Array<double, 3>>{
            {1.0, 2.0, 3.0},
            {}
        },
        VarLen<Array<double, 3>>{
            {-1, 42.0},
            {1, 2, 3}
        }
    };
    linear.writeDataSet("complex_nested", complexVlenArrayData);

    GenH5::CompData<VarLen<bool>> complexVlenBoolData{
        GenH5::VarLen<bool>{
            true, false, true, true
        },
        GenH5::VarLen<bool>{
            true,
        },
        { }
    };
    linear.writeDataSet("complex_vlen_bool", complexVlenBoolData);

    GenH5::CompData<VarLen<Version>> complexVlenVersionData{
        VarLen<Version>{
        },
        VarLen<Version>{
            Version::current(), Version::hdf5()
        },
        VarLen<Version>{
            42
        }
    };
    linear.writeDataSet("complex_vlen_version", complexVlenVersionData);



    auto multi = file.root().createGroup("Multidim");

    GenH5::CompData<int, int> compData2D{
        std::array<int, 10>{33, 42, 1, -213, 213, 21, 21, 23, 4, -124},
        std::array<int, 10>{213, 32, 21, 12, -1, -2, -230123, 1234, 32, 42}
    };
    compData2D.setDimensions({5, 2});
    multi.writeDataSet("compound_2d", compData2D);

    auto attr = file.root().createAttribute("my_ref",
                                            GenH5::DataType(H5::PredType::STD_REF_OBJ.getId()),
                                            GenH5::DataSpace::Scalar);
    auto ref  = dset.toReference();
    attr.write(&ref);

    char bitfield = 0b10101010; // = AA
    linear.createDataSet("bitfield",
                              GenH5::DataType::fromId(H5Tcopy(H5T_NATIVE_B8)),
                              GenH5::DataSpace::Scalar).write(&bitfield);
}
