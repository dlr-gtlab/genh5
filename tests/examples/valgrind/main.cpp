
#include "hdf5.h"
#include "genh5.h"

int main(int argc, char** argv)
{
    GenH5::File file("valgrind.h5", GenH5::Overwrite);

    GenH5::Group const& root = file.root();

    auto dset = root.writeDataSet0D("bla", 42);

    root.writeDataSet0D("ref", dset.toReference());

    return 0;
}
