# GTlab HDF5 Wrapper

This wrapper is build upon the HDF5 C/C++ API. It does not aim to cover all functionalities of the HDF5 data format but the main aspects. However the wrapper is fully compatibel with the C/C++ API.
A good introduction to the data format can be found here:

https://portal.hdfgroup.org/display/HDF5/Introduction+to+HDF5

## Functionalities

The wrapper covers the following aspects:
 - Creating, accessing and deleting files, groups, datsets and attributes
 - Chunked datasets and compression
 - Flexible template-based datatype interface for simple and comlex compound types (using `GtH5Data<...>`)
 - Simple interface for dataspaces
 - Simple interface for (de-) referencing nodes and attributes

Currently not covered:
 - Accessing dataset regions
 - External HDF5 files and datasets
 - Creating soft/hard links to nodes
 - Certain datatpyes (eg. opaque types, compound types containing more than three components or arrays)

## How to use the wrapper

### Notes
 - When instancing resources (eg. files, groups, datasets and attributes) use `GtH5Object::isValid` to check for correct allocation.

### Examples

Writing a list of Integers to a dataset:
```c++
// create simple int data
GtH5Data<int> data( {1, 2, 3, 4} );

// create the file (overwrite if it exists)
GtH5File file = GtH5File(QByteArrayLiteral("my_file.h5"), GtH5File::CreateOverwrite);
// access the root group of the file and create a subgroup
GtH5Group group = file.root().createGroup(QByteArrayLiteral("my_group"));
// create simple dataset at '/my_group/my_data'
GtH5DataSet dataset = group.createDataset(QByteArrayLiteral("my_data"), data.dataType(), data.length());
// validate the resource (should be called for each object)
if (!dataset.isValid()) {	
	return; // error handling
}
// write the data
dataset.write(data);
```

Reading a list of compound datatypes from a dataset (int, QString):
```c++
// open the file
GtH5File file = GtH5File(QByteArrayLiteral("my_file.h5"), GtH5File::OpenReadOnly);
if (!file.isValid()) {
	return; // error handling
}
// create data container
GtH5Data<int, QString> data;
// access the root group of the file and the desired dataset at '/my_group/my_data'
GtH5DataSet dataset = file.root().openDataset(QByteArrayLiteral("my_group/my_data"));
// check for correct dataset properties
if (!dataset.isValid() || dataset.dataType() != data.dataType() || dataset.dataSpace().sum() != 42) {
	return; // error handling
}
// read the data from the dataset
dataset.read(data);
// deserialize the data into separate lists
QVector<int> ints;
QStringList strings;
data.deserialize(ints, strings);
```

Referencing an attribute:
```c++
// create or open the file if it exists
GtH5File file = GtH5File(QByteArrayLiteral("my_file.h5"), GtH5File::CreateReadWrite);
// access the root group of the file and create an attribute
GtH5Attribute attr = file.root().createAttribute(QByteArrayLiteral("my_attribute"), GtH5Data<float>().dataType(), 1);
if (!attr.isValid) {
    return; // error handling
}
// create reference
GtH5Reference ref = attr.toReference();

// dereference
GtH5Attribute attr = ref.toAttribute(file);
```
