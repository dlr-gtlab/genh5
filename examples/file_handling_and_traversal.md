<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# File handling and internal path traversal

The following examples describe how to use API to open and create files, groups, datasets and attributes. 

> **Note:** try-catch blocks were omitted for the following examples

## File handling

HDF5 Files can be created or opened using the `GenH5::File` class. It implements a constructor, that accepts a file string (i.e. the target file path) and file access flags.

Via the file access flags one can define whether to open, create or overwrite the file. Two additional flags can be used to define read mode can be used: read-only or read-write. However these are only necessary in conjunction with the open flag. 

*Examples:*

```cpp
// opens as read-write. Throws if the file does not exist
GenH5::File file("my_file.h5");
// same as above           
GenH5::File file("my_file.h5", GenH5::Open);        
 // creates the file. Throws if the file already exists
GenH5::File file("my_file.h5", GenH5::Create);     
// Creates the file if it does not exist or overwrites it if it does 
GenH5::File file("my_file.h5", GenH5::Overwrite);   

// Creates the file if it does not exist or opens it if it does 
GenH5::File file("my_file.h5", {GenH5::Open | GenH5::Create}); 
// Opens the file in read only mode. Throws if the file does not exist 
GenH5::File file("my_file.h5", {GenH5::Open | GenH5::ReadOnly}); 
```

Once a file has been created or opened using `GenH5::File`, the object takes ownership of a handle, for the duration of its lifetime. It may be closed prematurely using `GenH5::File::close`.

The file oject can be used to retrieve the file name, file path or the root group in form of a `GenH5::Group&` instance using `GenH5::File::root`. This group object can be used to traverse the internal file hierarchy (see below for more details).

## Internal path travsersal

One can access the root group of a valid HDF5 File using `GenH5::File::root`. It can then be used to traverse the internal file hierarchy, create and access subgroups, datasets and attributes.

See [finding child nodes and attributes](find_child_nodes.md) for more details on iterating over and finding child nodes and attributes in a HDF5 file.

### Creating and accessing groups

To create a new group call `createGroup` on a group object (e.g. the root group) and pass the group name to the function.

> **Note:** If the group already exsits it will be opened instead.

```cpp
GenH5::Group const& root = file.root();
GenH5::Group subgroup = root.createGroup("my_group");
```

One can strictly open a group by using `openGroup` instead. This method will throw if the group does not exist:

```cpp
GenH5::Group subgroup = root.openGroup("my_group");
```

To check if a node exists use `GenH5::Location::exists`:

```cpp
root.exists("my_group");
// can also be used to check if a path exists
root.exists("my_group/my_dataset");
```

### Creating and accessing datasets

To create a new dataset call `createDataSet` on a group object (e.g. the root group).

One has to provide the dataset name as well as the target datatype and dataspace. The datatype defines the a dataelement, while the dataspace defines the layout (scalar entry, list of entries or multidimensional data layout).

A datatype can be created using the `GenH5::dataType<...>` template function. The actual datatype will be deduced from the C++ template type. Alternatively one can use the predefined datatypes and other static meber methods in `GenH5::DataType`. See [creating datatypes](creating_datatypes.md) for more details.

The dataspace can be created using `GenH5::DataSpace`. See [creating dataspaces and selections](creating_dataspaces_and_selections.md) for more details.

Optionally one can proivde an object of `GenH5::DataSetCProperties` to specify the dataset creation properties. This can be used to set enable chunking and compression.  

> **Note:** If the dataset already exsits this library will attempt to open it. It will check if the datatype and dataspace match and overwrite it if it does not.

```cpp
GenH5::Group const& root = file.root();
GenH5::DataSet dataset = root.createDataSet("my_dataset", GenH5::dataType<int>(), GenH5::DataSpace::Scalar);
```

One can open a dataset by using `openDataSet` instead. This method will throw if the dataset does not exist:

```cpp
GenH5::DataSet dataset = root.openDataSet("my_dataset"); 
```

To check if a node exists use `GenH5::Location::exists` (see above).

> **Note:** (See [reading and writing data to and from a dataset or attribute](reading_and_writing_data.md) for more details)
>
> One can use `writeDataSet(0D)` on a group object, to create a dataset and write data to it directly.
>
> Similarly one can use `readDataSet(0D)` to open a dataset and read data from it. 


### Creating and accessing attributes

Similary to creating datasets an attributes can be created using `createAttribute` on a node object (dataset or group). It also requires the attribute name, a datatype and dataspace, which can be retrieved using the methods explained above.

> **Note:** If the attribute already exsits this library will attempt to open it. It will check if the datatype and dataspace match and overwrite it if it does not.

```cpp
GenH5::Group const& root = file.root();
GenH5::Attribute attr = root.createAttribute("my_attr", GenH5::dataType<int>(), GenH5::DataSpace::Scalar);
```

One can open an attribute by using `openAttribute` instead. This method will throw if the attribute does not exist:

```cpp
GenH5::Attribute attr = root.openAttribute("my_attr"); 
```

It is also possible to open an attribute by path:

```cpp
GenH5::Attribute attr = root.openAttribute("my_dataset", "my_attr"); 
```

To check if a node has an attribute use `GenH5::Node::hasAttribute`:

```cpp
root.hasAttribute("my_attr");
```

> **Note:** (See [reading and writing data to and from a dataset or attribute](reading_and_writing_data.md) for more details)
>
> One can use `writeAttribute(0D)` on a node object, to create an attribute and write data to it directly.
>
> Similarly one can use `readAttribute(0D)` to open an attribute and read data from it. 
>
> There is also `writeVersionAttribute` and `readVersionAttribute` for creating and accessing a special version attribute instead.
