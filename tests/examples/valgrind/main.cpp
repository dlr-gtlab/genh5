
#include "hdf5.h"
#include "genh5.h"

int main()
{
    GenH5::File file("valgrind.h5", GenH5::Overwrite);

    GenH5::Group const& root = file.root();

    std::string test = "test";
    root.writeDataSet0D("bla", test);

    auto res = root.readDataSet0D<std::string>("bla");

    auto dset = root.openDataSet("bla");

    free(res.raw());


    return 0;
}
