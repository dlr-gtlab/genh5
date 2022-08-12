# GTlab HDF5 Wrapper

This wrapper is build upon the HDF5 C/C++ API. It does not aim to cover all functionalities of HDF5 but the main aspects. The wrapper is fully compatibel with the C/C++ API. 

The online documentation can be found here: https://at-twk.pages.gitlab.dlr.de/gtlab-hdf5-wrapper/index.html

A good introduction to HDF5 can be found here: https://portal.hdfgroup.org/display/HDF5/Introduction+to+HDF5

## Functionalities

The wrapper covers the following aspects:
 - Creating, accessing and deleting files, groups, datsets and attributes
 - Chunked datasets and compression
 - Flexible template-based datatype interface for simple and complex compound types (using `GenH5::Data` or `GenH5::CompData`)
 - Simple interface for dataspaces
 - Simple interface for (de-) referencing nodes and attributes

Currently not covered:
 - Accessing dataset regions
 - External HDF5 files and datasets
 - Creating soft/hard links to nodes
 - Certain datatypes (eg. opaque types, `bool`, `QPointf` etc.)

## How to use the wrapper

### Note
> The try-catch blocks in the following examples were omitted. All exceptions may be catched using `GenH5::Exception` or `std::runtime_error`

### Examples

Writing a list of Integers to a dataset:
```c++
// create simple int data
GenH5::Data<int> data{ 1, 2, 3, 4 };
// create the file or overwrite if it already exists
GenH5::File file{ "my_file.h5", GenH5::Overwrite);
// access the root group of the file and create a subgroup
GenH5::Group group = file.root().createGroup("my_group");
// create simple dataset at '/my_group/my_data'
GenH5::DataSet dataset = group.createDataset("my_data", data.dataType(), data.dataSpace());
// write the data
dataset.write(data);
```

Reading a list of compound datatypes from a dataset (int, QString):
```c++
// open the file
GenH5::File file{ "my_file.h5"), GenH5::ReadOnly };
// create data container
GenH5::CompData<int, QString> data;
// set compound type names
data.setTypeNames({"my_ints", "my_strings"});
// access the root group of the file and the desired dataset at '/my_group/my_data'
GenH5::DataSet dataset = file.root().openDataset("my_group/my_data");
// check for correct dataset properties
if (dataset.dataType() != data.dataType() || dataset.dataSpace().sum() != 42) {
	return; // error handling
}
// read the data from the dataset
dataset.read(data);
// deserialize the data into separate lists
std::vector<int> ints;
QStringList strings;
data.deserialize(ints, strings);
```

Referencing an attribute:
```c++
// create or open the file if it exists
GenH5::File file{ "my_file.h5", GenH5::ReadWrite);
// create data container
GenH5::Data<float> data{0f};
// access the root group of the file and create an attribute
GenH5::Attribute attr = file.root().createAttribute("my_attribute", GenH5::dataType<float>(), GenH5::DataSpace::Scalar);
// create reference
GenH5::Reference ref = attr.toReference();
...
// dereference
GenH5::Attribute attr = ref.toAttribute(file);
```
