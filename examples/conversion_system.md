<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# Conversion System

This library implements a flexible and generic way of mapping C++ types to HDF5 datatypes and provides a powerful conversion system for converting C++ types to an underlying type, that can be written to/read from HDF5. 

## Motivation

### Writing POD and Strings

Writing plain old data to HDF5 is as simple as:
```cpp
int my_int = 42;
dataset.write(&my_int);
```

However writing a single `std::string` (variable length string) to HDF5 requires an extra step, as we have to pass a pointer to the c-string:

```cpp
std::string s = "Hello World";
char const* c = s.c_str(); // access underlying c-string

dataset.write(&c, ...);
```

For wrtting a `QString`, the procedure is even more involved:

HDF5 currently supports ASCII and UTF8 as character sets, however `QString` uses the UTF16 encoding (`wchar`). 
`QString` can be converted to UTF8 using the `Qstring::toUtf8()` mehtod. 
The c-string of the resulting `QByteArray` can then be accessed using the `QByteArray::constData` method. 
However the temporary byte array must be stored in a separate variable, as the c-string would become a dangling pointer as soon as the byte array is deleted. 
Thus, we have to adapt the snippet like this:

```cpp
QString s = "Hello World";
QByteArray b = s.toUtf8(); // convert to utf8
char const* c = b.constData(); // access underlying c-string

dataset.write(&c, ...); 
```

When writing a list of strings to HDF5 these steps are complicated still, as the underlying c-strings are not in a contiguous memory layout. 
Hence, we would have to create a list/vector of `const char*`, which then may be written to HDF5. 
As seen above, when dealing with list of `QString` we also have to create a separate list for the temporary `QByteArray` objects to keep the c-strings valid for writing.

We could do with a helper method, to write `std::string`, `QString` and similar string classes to HDF5, however, ideally the library should provide a simple solution for such problems.

### Compound Data

> See [creating datatypes](creating_datatypes.md) for more details.

In HDF5, datatypes can be combined into compound types. 
To read and write data to HDF5, we must arrange it in a contiguous memory layout. 
If we have a compound type consisting of two members (`A` and `B`), the members have to be arranged like this: `A` `B` `A` `B`...

For the following example we conside a compound type of a variable string and a floating point value. 
These can be arranged in a helper struct:

```cpp
struct MyData
{
    char* string; // variable string
    double value; // floating point value
};
```

Creating a single compound data element is rather straight forward except for the extra step when converting a string (see above). 
However again these extra steps accumulate, when we want to create a list of compound data:

```cpp
QStringList strings{...};
QVector<double> doubles{...};

int length = strings.length();
assert(length == doubles.lenght();

// buffer for strings
QVector<QByteArray> buffer;
buffer.reserve(length);

// final data vector
QVector<MyData> data;
data.reserve(length);

for (int i = 0; i < length; ++i)
{
    // store temporary utf8 string
    buffer.append(strings[i].toUtf8());
    data.append(MyData{ 
        buffer[i].data(), 
        doubles[i] 
    });
}

// write `data` to the HDF5 dataset:
dataset.write(data.constData(), ...); 
```

### Datatype

The memory layout of the data in an HDF5 dataset or attribute are defined by HDF5 datatypes. 
These datatypes must be passed to the HDF5 library when creating a dataset or attribute. 

Say we want to write a vector of integers (`std::vector<int>`). 
We have to explicitly tell HDF5 that we are writing a list of integers (here: `NATIVE_INT`).
However, we (and the compiler) already know that we want to write a list of integers, after all it is engraved into the C++ type.
It would be nice if we could use this C++ type directly to refer to the HDF5 type without too much effort, like mapping `int` to `NATIVE_INT` etc. 

## Goals

1) It should be possible to convert user-defined types that cannot be written directly to HDF5 (like `std::string`, `QByteArray` or `QString`) to an underlying/storage type. 

2) When converting the data it may be necessary to buffer temporary objects (e.g. `QByteArray` for `QString`).

3) The library should deduce the HDF5 datatypes automatically from a C++ type

## Data Conversion

### Conversion Types

This library implements a system to map user-defined types to a storage/conversion type, that can be used to write to and read from HDF5 datasets.

The conversion type can be access using `GenH5::conversion_t`. By default all types are mapped to itself:

```cpp
GenH5::conversion_t<int>;           // == int
GenH5::conversion_t<double>;        // == double
GenH5::conversion_t<struct MyType>; // == MyType
```

A conversion type can be mapped to a different type using the macro `GENH5_DECLARE_CONVERSION_TYPE(NATIVE, CONVERTED)`. 

This library already maps a few different types (e.g. `std::string`, `QByteArray` and `QString`):

```cpp
GenH5::conversion_t<std::string>;   // == char*
GenH5::conversion_t<QByteArray>;    // == char*
GenH5::conversion_t<QString>;       // == char*
```

The macro should be placed at the top of an file, after including `genh5_conversion.h` and before using the conversion system:

```cpp
#include "genh5_conversion.h"

struct MyType { ... };

GENH5_DECLARE_CONVERSION_TYPE(MyType, int)

static_assert( 
    std::is_same< GenH5::conversion_t<MyType>, int >::value, 
    "These types should match!"
);
```

> Default conversions are defined in [src/genh5_conversion/defaults.h](../src/genh5_conversion/defaults.h).
>
> Conversions for the STL and Qt are defined in [src/genh5_conversion/bindings.h](../src/genh5_conversion/bindings.h).

### Buffer Types

As noted earlier some conversions require the buffering of temporary objects. 
For this a similar system is used, in which types are mapped to buffer types.

It can be access using `GenH5::buffer_element_t`. 
Again, by default all types are mapped to each other. 

To specify a dedicated buffer type the macro `GENH5_DECLARE_BUFFER_TYPE(TYPE, BUFFER)` can be used. 
Similarly, it should be placed at the top of an file and before using the conversion system.

### Conversion

For now we only mapped types to other types; no conversion took place yet. 

For converting data from a user type to the storage type (i.e. data that can be written to HDF5) this library provides the `GenH5::convert()` method. 
It is used internally before writing to a dataset. 
Thus, there is generally no need to call it explicitly. 
There are overloads available for each type that is currently supported. 
However, there is no default conversion implement for new types.

If a user-defined type can be implicitly converted to its conversion type, use the macro `GENH5_DECLARE_IMPLICIT_CONVERSION(TYPE)` to register a new overload:

```cpp
GENH5_DECLARE_IMPLICIT_CONVERSION(MyType)
```

However, if there is not an implicit conversion available, use `GENH5_DECLARE_CONVERSION(TYPE_FROM, VALUE_NAME, BUFFER_NAME)`. 
The macro accepts three arguments:
1) The type to convert from. It may use qualifiers like `const&`.
2) The variable name of the value to convert (e.g. `value`).
3) The variable name of the buffer (e.g. `buffer`). 
Buffer is a container, to which a temporary object may be appended. 
The buffer does not have to be used if the conversion does not require any buffering. 

Then, the conversion must be implemented:

```cpp
GENH5_DECLARE_CONVERSION(MyType const&, value, buffer)
{
    Q_UNUSED(buffer)
    return value.getInt();
}
```

The `GenH5::convert()` function will be invoked internally by the library.

### Reverse Conversions

After a dataset has been read the "raw" data must be converted into the original/native type.
For this purpose the library implements the method `GenH5::converTo<T>()` function which uses an implicit conversion by default.
 
However for some types there may not be any implicit conversions available.

One may use the macro `GENH5_DECLARE_REVERSE_CONVERSION(TYPE_TO, TYPE_FROM, VALUE_NAME)` to implement a dedicated reverse conversion.
The macro accepts three arguments:
1) The orignal type to convert to.
2) The type to convert from.
3) The variable of the value to convert from.

```cpp
GENH5_DECLARE_REVERSE_CONVERSION(MyType, int, value)
{
    return MyType::fromInt(value);
}
```

The `GenH5::convertTo` function will be invoked internally by the library.

### Generic Conversion

Once the conversion methods are implemented the type should also work with Array-, VarLen- and Compound-data. For these the following typedefs are defined:
- `GenH5::Array` for array types
- `GenH5::VarLen` for variable length types
- `GenH5::Comp` for compound types

They can be used like this:

```cpp
using GenH5::convert;

GenH5::VarLen<MyType> vlenValue;
GenH5::buffer_t<GenH5::VarLen<MyType>> buffer;

hvl_t convData = convert(vlenValue, buffer);
```

## Datatype Deduction

This library provides the `GenH5::dataType<T>()` function, which allows returns the HDF5 datatype associated with the template argument(s).

To make this possible a similar system was implemented, which allows to map C++ types to HDF5 datatypes.

This is done using the macro `GENH5_DECLARE_DATATYPE(NATIVE_TYPE, H5_TYPE)`. It accepts two arguments:
1) The C++ type.
2) The target HDF5 type.

```cpp
GENH5_DECLARE_DATATYPE(MyType, GenH5::dataType<int>());
```

If the HDF5 type is more complex and cannot be declared inline, use the macro `GENH5_DECLARE_DATATYPE_IMPL(NATIVE_TYPE)`.

The desired datatype can then be implemented in a function-like manner:

```cpp
GENH5_DECLARE_DATATYPE_IMPL(MyType)
{
    return GenH5::dataType<int>();
}
```

It may then be used like this:

```cpp
GenH5::DataType dtype = GenH5::dataType<MyType>();
```

Once the datatpye has been defined it should also work with Array-, VarLen- and Compound-datatypes out of the box:

```cpp
GenH5::dataType<GenH5::Array<MyType, 42>>();
GenH5::dataType<GenH5::VarLen<MyType>>();
GenH5::dataType<GenH5::Comp<MyType, ...>>();
```
