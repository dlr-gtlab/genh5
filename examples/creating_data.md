# Data API 

This library provides multiple template based data classes adn typedefs, that should generally be used for reading and writing data to datasets or attributes. 
They implement different helper methods to provide convenience when dealing with any kind of data.

In general there a five different types (and typedefs) that should be used:
- `GenH5::Data`
- `GenH5::Data0D`
- `GenH5::CompData`
- `GenH5::CompData0D`
- `GenH5::FixedString0D`

> **Note:** Most data classes use a static buffer instance internally as an optimization. This is not thread safe and can be deactivated by defining `GENH5_NO_STATIC_BUFFER` globally when using Genh5.
>
> **Note:** try-catch blocks were omitted for the following examples

## Simple Data

The `GenH5::Data` template type should be used most of the times when reading or writing 1D or multi dimensional data. It can support compund types as well by using multiple template types (see "Compound Data" for more details).

The template works like a typical vector class and implements all expected functionalities:  

- interators
- push_back/append
- getters for size
- last and first entry
- index operators as well as 
- reserve and resize etc.

### Appending data:

> **Note:** The library uses the typedef `GenH5::Vector` for vector types, which is (currently) a `QVector`.

Internally the template class will always use a 1D vector. However it can be used to write multidimensional data as well. Data can be set as usual:

```cpp
GenH5::Data<int> myInts{1, 2, 3, 4, 5};
// optional: resvere enough space beforehand
myInts.reserve(12);
// append works with different input types
myInts.append(std::vector<int>(6, 7, 8));
myInts.append(QVector<int>(9, 10, 11));
myInts.append(12);
```

### Accessing dataspace and datatype:

`GenH5::Data` (and all other helper classes) provide a method for creating an appropiate dataspace using the `dataSpace` mehtod as well as a datatype using the `dataType` method. 
The dataspace will be 1D by default and will have the same number of elements as there are in the data object. 
The datatype will  be deduced by the template type(s). 

It can therefore be used in conjunction with `GenH5::Group::createDataSet` or `GenH5::Node::createAttribute` to create a desired dataset or attribute.

```cpp
GenH5::Group const& root = file.root();
GenH5::Attribute attr = root.createAttribute("my_attr", myInts.dataType(), myInts.dataSpace());
attr.write(myInts);
```

### Conversion functions:

Note that the template class does not always use the template type internally. For example, when dealing with strings like `std::string` a `char*` is used internally. Iterating over the data object this way, would only iterate over the internal `char*` (see [the conversion system](conversion_system.md) for more details). To access the data as its template type use `value`, `values` or `unpack` instead.

- `value` functions identically to the index operator or `at` and requires a index to fetch and will a return a single value (e.g. `std::string`). 
- `values` will return all data as a converted list (e.g. `GenH5::Vector<std::string>`).
- `unpack` accepts most container types and writes the data directly to it.

```cpp
GenH5::Data<std::string> data{...};

std::string str                 = compData.value(42);
GenH5::Vector<std::string> strs = compData.values();
std::vector<std::string> strs   = compData.values<std::vector<std::string>>(); // custom return type

QList<std::string> myStrs;
data.unpack(myStrs);
```

### Scalar Data:

`GenH5::Data0D` is a helper class to read/write scalar data more easily. It works similarly to `GenH5::Data` except it does not have a `values` or `unpack` method and does not use a vector internally.

### Multidimensional Data:

As mentioned before `GenH5::Data` can also be used to write multidimensional data. This can either be done by setting a custom dataspace when creating a dataset or attribute or by using the `setDimensions` method.

> **Note:** `setDimensions` requires that the number of elements equals or exceeds the total dimension size, otherwise the function will throw. In other words: call this method after all data has been appended. 

The method requires a `GenH5::Dimension` vector as an argument. The data will not be altered or truncated, its simply a internal property on how to interpret the data.

Once set, the `dataSpace` method will return a dataspace with the same dimensions. Additionally overlaods of the index operator, the `at` and `value` method can be used, that accept multiple indicies. By default there is an overload that accepts two indicies for 2D data (except for the index operator). For higher ordered data, a list of indicies can be supplied:

```cpp
GenH5::Data<std::string> data{...};

data.setDimensions({4, 6}); // make sure that data has 4 * 6 elements

char* cstr2D      = data.at(1, 2);    // element at data[1][2]
std::string str2D = data.value(1, 2); // value at data[1][2]

data.setDimensions({2, 3, 4}); // make sure that data has 2 * 3 * 4 elements

char* cstr3D_1    = data[{1, 2, 3}];       // element at data[1][2][3]
char* cstr3D_2    = data.at({1, 2, 3});    // element at data[1][2][3]
std::string str3D = data.value({1, 2, 3}); // value at data[1][2][3]
```

The dimensions set can be retrieved using the `dimensions` method. It will return an empty list, if no explicit dimensions were set. Use `clearDimensions` to clear the dimensions property.

### Utility functions:

- To access the underlying storage container as reference use the `raw` method (in most cases `GenH5::Vector` i.e. `QVector`).

- Last but not least `GenH5::Data` implements a `split` method, that splits the data into sublists. By default `GenH5::Vector<GenH5::Vector<T>>` will be used, but this can altered using a custom function template type. It requires a size type as parametter defining the length of the sublists. This method is especially useful to convert array data into a nested list (e.g. `GenH5::Data<GenH5::Array<T, N>>` into `GenH5::Vector<GenH5::Vector<T>>`)

     > **Note:** The `split` method will throw, if the sublist size does not fit into the size of the container size (`data.size() % n == 0`) 

## Compound Data

As stated previously `GenH5::Data` also works for compound data by using multiple template types:

```cpp
GenH5::Data<double, std::string> compData;
```

There is also the type alias `GenH5::CompData`, which is identical to `GenH5::Data` but only if, multiple template arguments are used. Otherwise `GenH5::CompData<T>` will still always return a compound type while `GenH5::Data<T>` would return the plain datatype of `T`.

> **Note:** All aspects previously described in "Simple Data" will apply here as well

### Appending data:

> **Note:** `GenH5::Comp` is used internally and is simply an alias for `std::tuple`

`GenH5::CompData` uses an `std::tuple` internally to create a contiguous struct like memory layout using the template arguments. However `std::tuple` will layout the data in reverse order. For example, say we have the tuple type `std::tuple<A, B>`, then the offset of `B` is smaller than the offset of type `A`. Since this would mess with the expected datatype order, GenH5 will use reversed tuple types internally.

To append data to a compound data object we can insert the values in different ways:
- using the storage type (reversed tuple e.g. `std::tuple<char*, double>`)
- in the normal/expected order in form of the template types (e.g. `std::tuple<double, std::string>`)
- separate lists *of the same size* (otherwise the function may throw, e.g. `std::vector<double>, std::vector<std::string>`)
- separate arguments (e.g `double, std::string`)

```cpp
GenH5::CompData<double, std::string> compdata;

char cstring[] = {"by storage type"};
compdata.append(std::make_tuple(cstring, 12.0));
compdata.append(std::make_tuple(42.0, std::string("by value type")));

compdata.append(QVector<double>{1, 2, 3}, QVector<std::string>({"separate", "lists", "!"}));
compdata.append(-1.1234, std::string("single arg"));
```

### Compound datatype:

`GenH5::CompData` also implements the same `dataType` method for accessing the HDF5 compound datatype. There is also an overload that accepts the member names, from which the datatype will be created:

```cpp
GenH5::DataType dtype = compdata.dataType({"doubles", "strings"});
```

The member names can also be set beforehand using the `setTypeNames` method. It either accepts a list of compound member names or a datatype, from which the names will be deduced automatically:

```cpp
compdata.setTypeNames({"doubles", "strings"});
GenH5::DataType dtype = compdata.dataType(); // will have the member names set correctly
```

The typenames can be accessed using the `typeNames` method.

### Conversion functions:

Similar to `GenH5::Data` the methods `value`, `values` and `unpack` exists as well. However `value` and `values` will return a tuple type instead, which may not be as useful.
Therefore the template data class implements additionally a `getValue` and `getValues` method. These methods work identically to the ones described before, except that they require a template index for accessing the nth tuple element. `unpack` also accepts multiple arguments, "unpacking" the tuples into separate lists.

> **Note:** `getValue` also works with multidimensional data using multiple indicies.
>
> **Note:** Template indicies of `getValue` and `getValues` will behave as expected where `0` would point to the first template type.

```cpp
GenH5::CompData<double, std::string> compData{...};

std::string str               = compData.getValue<1>(42); // same as std::get<1>(compData.value(42))
GenH5::Vector<double> doubles = compData.getValues<0>();
QList<double> doubles2        = compData.getValues<0, QList<double>>(); // custom return type

std::vector<double> doublesUnpacked;
std::vector<std::string> strsUnpacked;
data.unpack(doublesUnpacked, strsUnpacked);

double doubleUnpacked{};
std::string strUnpacked;
data.unpack(42, doubleUnpacked, strUnpacked); // unpacks the 42 idx into both arguments
```

`GenH5::CompData` also implements a `setValue` method for overwriting a sepcific index. This index must exist.

### Scalar Compound Data:

Same as before `GenH5::CompData0D` is the data class for scalar compound data. It only implements the `getValue`, `unpack` method as well as a `setValue` method for assigning compound data.

## Fixed String Data

> **Note:** `GenH5::String` is used internally for string types and is (currently) an alias for `QByteArray`

Last but not least GenH5 also implements a helper class for reading and writing scalar fixed-string data. By default all string types will be interpreted by GenH5 as a variable string (e.g. `GenH5::dataType<std::string>()` will return a variable string datatype). The only exception is and char-array type that will be transformed into a fixed-string type (e.g. `GenH5::dataType<char[42]>()` is a fixed string datatype with 42 elements). However one does not always know the length of the fixed string datatpye at compiletime. For this `GenH5::FixedString0D` can be used. It is used similarly to the other template types and implements much of the functionalities, but it does not require a template type. 

```cpp
GenH5::FixedString0D fixedStr{"Hello world"}; // accepts string literals
fixedStr.size();       // always 1!
fixedStr.stringSize(); // here: 11 elements, trailing \0 not counted

fixedStr = std::string("12345");
fixedStr.size();       // always 1!
fixedStr.stringSize(); // here: 5 elements, trailing \0 not counted

GenH5::String str             = fixedStr.value();
std::string str               = fixedStr.value<std::string>(); // custom return type
```

## Array Data

To create a data struct for an array datatype one can use the `GenH5::Array` typedef. It accepts a template type as well as the length of the array data (which must be known at compiletime):

```cpp
using MyArrayType = GenH5::Array<int, 3>;
```

It can then be used in conjunction with the previously discussed `GenH5::Data(0D)` and `GenH5::CompData(0D)` typedefs to create array data:
```cpp
GenH5::Data<GenH5::Array<int, 3>> data{
   {1, 2, 3},
   {4, 5, 6},
   {7, 8, 9}
};
```

> **Note:** Alternatively one can use `T[N]` instead of `GenH5::Array<T, N>`

## VarLen Data

To create variable length data for a variable length HDF5 datatype use the typedef `GenH5::VarLen`. Similarly to the `GenH5::Array` typedef it can be used in conjunction with the previously discussed `GenH5::Data(0D)` and `GenH5::CompData(0D)` typedefs to create varlen data:
```cpp
GenH5::Data<GenH5::VarLen<int>> data{
    {1, 2, 3, 4},
    {5, 6},
    {7,},
    {8, 9}
};
```
