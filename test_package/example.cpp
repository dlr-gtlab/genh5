#include <genh5_data.h>
#include <genh5_file.h>
#include <genh5_dataset.h>

#include <iostream>

int main()
{

    // create simple int data
    GenH5::Data<int> data{ 1, 2, 3, 4 };
    // create the file or overwrite if it already exists
    GenH5::File file("my_file.h5", GenH5::Overwrite);
    // access the root group of the file and create a subgroup
    GenH5::Group group = file.root().createGroup("my_group");
    // create simple dataset at '/my_group/my_data'
    GenH5::DataSet dataset = group.createDataset("my_data", data.dataType(), data.dataSpace());
    // write the data
    dataset.write(data);

    return 0;
}
