<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# Creating Datatypes

The following example shows how to use this library to create both simple and complex compound datatypes.

When dealing with HDF5 datatypes the library offers two approaches for creating a datatype:
1) **Directly** using explicit methods for creating the desired dataype.
2) **Generically** using templates from which the datatype is deduced automatically.

In general the latter approach should be used more often, as its simply more convenient and straight forward to use. 

However, we will start using the direct way in order to better explain how datatypes work in HDF5 and GenH5.

> **Note:** try-catch blocks were omitted for the following examples

## Direct Datatype Creation

### Predefined Types

> Not all datatypes of the native HDF5 library are covered currently. Feel free to create an issue to support a missing type.

HDF5 predefines different datatypes, which make up the most essential building blocks. 
The library currently forwards the following native datatypes:

```ccpp    
GenH5::DataType::Bool        // == bool
GenH5::DataType::Char        // == char
GenH5::DataType::Int         // == int
GenH5::DataType::Long;       // == long int
GenH5::DataType::LLong;      // == long long int
GenH5::DataType::UInt;       // == unsigned int
GenH5::DataType::ULong;      // == unsigned long int
GenH5::DataType::ULLong;     // == unsigned long long int
GenH5::DataType::Float;      // == float
GenH5::DataType::Double;     // == double
```

For null-terminated (utf8) strings, the `GenH5::DataType::VarString` may be used. 
If a string of fixed size or non utf8 encoding is required the `GenH5::DataType::string` method may be used.

One may check if a string datatype is null-terminated (variable length) with the `GenH5::DataType::isVarString` method. 

### Compound Types

> When reading/writing data to HDF5 the memory buffer must be contiguous. 

HDF5 allows for the creation of compound datatypes. 
Each member of a compound type must follow a struct-like layout to archive a contiguous memory layout similar to this:

```cpp
struct MyData
{
    typename A;
    typename B;
    typename C;
    // ...
};
```

For the following example we conside a compound type of a variable string and a floating point value. 
The struct would then look like this:

```cpp
struct MyData
{
    char* string; // variable string
    double value; // floating point value
};
```

To create the compound type the method `GenH5::DataType::compound()` can be used. It requires two arguments. 
1) The first denotes the size of the compound element (i.e. the size of `MyData`).
2) The second argument is a list of all members.
Each member is of type `GenH5::CompoundMember` and is defined by a name, an offset inside the memory layout of the compound element and its datatype (which itself may be any kind of datatype).

```cpp
auto type = GenH5::DataType::compound(sizeof(MyData), {
	GenH5::CompoundMember{"my_string", offsetof(MyData, string), GenH5::DataType::VarString},
	GenH5::CompoundMember{"my_value",  offsetof(MyData, value),  GenH5::DataType::Double},
};
```

To check if a datatype is a compound, use `GenH5::DataType::isCompound()`. 
The method `GenH5::DataType::compoundMembers()` return a list of all members. 
The total byte size of the compound type is accessed by invoking `GenH5::DataType::size()`.

### Array Types

In HDF5 a datatype can also be used to create fixed-sized array types representing a list of data elements.

This library provides the `GenH5::DataType::array()` method. It accepts two arguments:
1) The datatype to chain.
2) The size for 1D arrays or a dimension vector for ND arrays.

The following snippet creates an integer array type, with the dimensions 7 x 6 (42 elements):

```cpp
auto type = GenH5::DataType::array(GenH5::DataType::Int, {7, 6});
```

Use `GenH5::DataType::isArray` to check if a datatype is an array type. 
The method `GenH5::DataType::arrayDimensions` will return the dimensions of the array type (e.g. a 1D vector for a 1D array).
The type which makes up the array can be retrieved using `GenH5::DataType::superType`.


### Varlen Types

Whereas an array-type has a fixed length for each entry, a varlen type can be used for data in which each entry has a varying length.

The `GenH5::DataType::varLen()` method can be used:

```cpp
auto type = GenH5::DataType::varlen(GenH5::DataType::Float);
```

The type which makes up the varlen type can be retrieved using `GenH5::DataType::superType`.

When writing varlen data, the `hvl_t` struct can be used to set the length and a pointer to the data. 
Alternatively this library provides the `GenH5::VarLen` typedef that may be used in conjunction with `GenH5::Data<...>` to create varlen data more easily. 
See [reading and writing data from and to datasets](dataset_read_write.md) for more information.

## Template-based Datatype Deduction

Whereas the direct approach can be used to create any HDF5 datatype (especially at runtime); the library provides another, more generic way of creating datatypes.

For this purpose the `GenH5::dataType()` function can be used.

### Simple Types:

Datatypes like `int`, `size_t`, `char*`, `bool` etc. can be used as template arguments and are automatically converted into the corresponding HDF5 datatype:

```cpp
GenH5::dataType<int>()    // == GenH5::DataType::Int;
GenH5::dataType<bool>()   // == GenH5::DataType::Bool;
GenH5::dataType<double>() // == GenH5::DataType::Double;
GenH5::dataType<char>()   // == GenH5::DataType::Char;
GenH5::dataType<char*>()  // == GenH5::DataType::VarString;
```

The same goes for used-defined types like `std::string`, `QString` and `QByteArray` for varlen strings.

The library also provides a way of registring new datatypes for used-defined classes or structs (see the [conversion system](conversion_system.md) for more details). 

### Compound Types:

For creating compound types multiple template arguments can be used.
Optionally, a list of names may be passed to name each member respectively.

```cpp
auto type = GenH5::dataType<QString, double>({"my_string", "my_value"});
```

Alternatively one may also use the `GenH5::Comp` typedef:

```cpp
auto type = GenH5::dataType<GenH5::Comp<QString, double>>({"my_string", "my_value"});
```

Internally the function uses a `std::tuple` to create a contiguous layout. It calculates the offsets to each member automatically. If no names are provided the members are named `"type_0"`, `"type_1"` etc.

> When using `GenH5::dataType<...>` its ill-advised to use a user-defined struct as a memory buffer for reading/writing a dataset, as the size and offsets may not be identical. 
Use `GenH5::CompData<...>` instead.

### Array Types

Creating 1D array types is as simple as using the `GenH5::Array` typedef. It requires the size as an template argument at compile time.

```cpp
auto type = GenH5::dataType<GenH5::Array<int, 10>>();
```

Alternatively one may use a c-sytle array as an template argument:

```cpp
auto type = GenH5::dataType<int[10]>();
```

### Varlen Types

Similarly a varlen type can be created using the `GenH5::VarLen` typedef:

```cpp
auto type = GenH5::dataType<GenH5::VarLen<float>>();
```

### Complex Types

This approach can be combined to create complex types very easily:

```cpp
// using declarations to shorten method call
using GenH5::VarLen;
using GenH5::Array;
using GenH5::Comp;

auto type = 
GenH5::dataType< 
    Comp<
      QString,
      VarLen<int>,
      Array<
        Comp<
          int,
          int
        >, 3>
      >
    >({
      "my_string", 
      "my_varlen_int", 
      "my_comp_int_array"
    }
);
```

> **Note:** One downfall when using nested *compound types* is that nested compound types cannot be named directly using the function arguments of the `GenH5::dataType<...>` function.
They are named `"type_0"`, `"type_1"` etc. by default instead.
