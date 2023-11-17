<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# Creating Dataspaces and Selections

The following example shows how to use this library to create dataspaces and selections.


### Dataspaces

A dataspace defines the layout of the data inside a dataset or an attribute. A dataspace can be distinguished in four different types:

- A **Null** dataspace does not hold any data (size of `0`). 
  A default constructed dataspace is null, but it may also be explicitly created using `GenH5::DataSpace::Null`. 
  One may check if an exisitng dataspace is null by calling the `GenH5::DataSpace::isNull` method. 

- A **Scalar** dataspace holds exactly one entry (size of `1`).
  It can be constructed using `GenH5:.DataSpace::Scalar`. 
  It may be checked with the `GenH5::DataSpace::isScalar` method. 
  `GenH5::DataSpace::dimensions` will return an empty dimension vector.

- A **Liniar** dataspace defines a 1D layout of a specific length (may be `0`, `1`, `42` etc.).
  It can either be instantiated using the `GenH5::DataSpace::linear` method or the `GenH5::DataSpace{n}`. 
  The method `GenH5::DataSpace::dimensions` will return the 1D dimension vector respectively.

- A **Multidimensional** dataspace has a ND data layout and can be constructed using a dimension vector. 
  `GenH5::DataSpace::dimensions` will return the dimension vector respectively. It can be created using `GenH5::DataSpace{n, m, ...}`. 
  
The size (i.e. the total number of elements stored) can be retrieved using the `GenH5::DataSpace::size` method. 

The number of dimensions can be retrieved using `GenH5::DataSpace::nDims` while the dimension vector can be accessed by calling `GenH5::DataSpace::dimensions`

### Selections

> See [reading from or writing to a subset of a dataset](https://portal.hdfgroup.org/display/HDF5/Reading+From+or+Writing+To+a+Subset+of+a+Dataset) for an indepth tutorial to data selections.

A data selection may be created using the function `GenH5::makeSelection` or by constructing an object of type `GenH5::DataSpaceSelection` directly. 

The selection itself will be stored in a dataspace, which can be accessed using `GenH5::DataSpaceSelection::space`. 
Once the selection has been committed, the total number of selected items can be retrieved using `GenH5::DataSpace::selectionSize`.

The helper class takes the original dataspace as a first argument (i.e. the dataspace defining the whole data layout).
If the selection count was not specified, the whole dataspace will be selected. 
By default the offset will be set to `0`, while the stride and block dimensions will be set to `1`.
If the number of dimensions of a selection property does not match the number of dimensions of the dataspace, each missing dimension will be filled up using its default value.

### Example

> **Note:** try-catch blocks were omitted for the following examples

#### Selecting every second element

Consider a dataspace with the dimensions `6 x 10`. The code to select every second element, skipping the first dimension, could look like this:

```cpp
GenH5::DataSpace space{6, 10};

auto selection = GenH5::makeSelection(space, 
                                      {5, 5},  // count:  select 5 x 5 elements 
                                      {1},     // offset: skips the first dimension (same as {1, 0})
                                      {1, 2}); // stride: select every second element in the second dimension

selection.space().selectionSize();             // == 5 x 5
```
Resulting selection visualized (x = selected):

```
 O O O O O O O O O O
 x O x O x O x O x O
 x O x O x O x O x O
 x O x O x O x O x O
 x O x O x O x O x O
 x O x O x O x O x O
```

#### Selecting first and last few elements

Consider a dataspace with the dimensions `8 x 6`. 
We can write a selection pattern to select the first and last elements few of each dimension:

```cpp
// first we have to create the data
GenH5::File file("test_file.h5", GenH5::Overwrite);

GenH5::Data<QString> data{
	"0", "1", "-", "-", "2", "3",
	"4", "5", "-", "-", "6", "7",
	"-", "-", "-", "-", "-", "-",
	"-", "-", "-", "-", "-", "-",
	"-", "-", "-", "-", "-", "-",
	"-", "-", "-", "-", "-", "-",
	"8", "9", "-", "-", "A", "B",
	"C", "D", "-", "-", "E", "F"
};
// set data dimensions
data.setDimensions({8, 6});

// write the data
GenH5::DataSet dset = file.root().writeDataSet("2D_selection", data);

// clear data
data.clear()

// create the desired selection
auto selection = GenH5::makeSelection(dset.dataSpace(), 
                                      {2, 2},  // we want to read two blocks of each dimension
                                      {0, 0},  // no offset
                                      {6, 4},  // distance to next block
                                      {2, 2}); // we want to read two rows and columns

// read selection
dset.read(data, selection); 
```

The data that was read will now contain all hex numbers from `0 - F` as a 1D list (16 elements):

```cpp
data = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"}
```
