# Conversion System

This library implements a flexible and generic way of mapping c++ types to HDF5 datatypes and 
provides a powerful conversion system for converting c++ types to an underlying type, that can be written to and read from HDF5. 

## Motivation

### Writing POD and Strings

Writing plain old data to HDF5 is as simple as:
```c++
int my_int = 42;
dataset.write(&my_int);
```

However writing a single `std::string` (variable length string) to HDF5 requires an extra step, as we allways have to pass a pointer to the data:

```c++
std::string s = "Hello World";
char const* c = s.c_str(); // access underlying c-string

dataset.write(&c, ...);
```

For wrtting a `QString`, the procedure is a also little more involved:

HDF5 currently supports ASCII and UTF8 as character sets, however `QString` uses the UTF16 encoding (`wchar`). 
`QString` can be converted to UTF8 using the `Qstring::toUtf8` mehtod. 
The c-string of the resulting `QByteArray` can then be accessed using the `QByteArray::constData` method.
However the temporary `QByteArray` must be stored in a separate variable, as the c-string will be invalid as soon as the object is deleted. 
Our snippet will have to look like this:

```c++
QString s = "Hello World";
QByteArray b = s.toUtf8(); // convert to utf8
char const* c = b.constData(); // access underlying c-string

dataset.write(&c, ...); 
```

These extra steps required accumulate when writing a list of `std::string` to HDF5, as the underlying strings are not in a contiguous memory layout. 
We would have to append each c-string to a vector of `char*` which then may be written to HDF5.
However when writing a list of `QString` we do not only have to create a vector of `char*` but also create a separate list for the temporary `QByteArray` to keep the c-strings valid.

We could do with a helper method, to write `std::string`, `QString` and similar string classes to HDF5, but what if the library could do this for us?

### Compound Data

> See [creating datatypes](creating_datatypes.md) for more details.

In HDF5 datatypes can be group to create compound types. 
In a compound dataset each entry must consist of an data element for each compound member. 
To read and write data it must have a contiguous memory layout, which can be archived by using a helper struct.

For the following example we conside a compound type of a variable string and a floating point value:

```c++
struct MyData
{
    char* string; // variable string
    double value; // floating point value
};
```

Creating a single compound data element is rather straight forward except for the extra step when converting a string (see above). 
However the extra steps stack, when we want to create a list of compound data:

```c++
// both list must share the same length
QStringList strings{...};
QVector<double> doubles{...};

int length = strings.length();

// buffer for strings
QVector<QByteArray> buffer;
buffer.reserve(length);

// data vector
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
```

### Datatype

The memory layout of the data in a HDF5 dataset or attribute are defined by the HDF5 datatypes. 
These datatypes must be passed to the hdf5 library when creating a dataset or attribute. 

Say we want to write a vector of integers (`std::vector<int>`). We know that we want the HDF5 datatype representing an integer (`NATIVE_INT`), after all its "engraved" into the c++ type.
It would be nice if we could use this c++ type directly to refer to the HDF5 type. Like mapping `int` to `NATIVE_INT`.

## Goals

1) It should be posstible to convert user-defined types that cannot be written directly to HDF5 (like `std::string`, `QByteArray` or `QString`) to an underlying/storage type. 

2) When converting the data it may be necessary to buffer temporary objects (e.g. `QByteArray` for `QString`).

3) Deduce the HDF5 datatype automatically from a c++ type

## Data Conversion

### Conversion Types

This library implements a system to map user-defined types to a storage/conversion type, that can be used to write to and read from HDF5 datasets.

The conversion type can be access using `GenH5::conversion_t`. By default all types are mapped to itself:

```c++
GenH5::conversion_t<int>;           // == int
GenH5::conversion_t<double>;        // == double
GenH5::conversion_t<struct MyType>; // == MyType
```

A conversion type can be mapped to a different type using the macro `GENH5_DECLARE_CONVERSION_TYPE(NATIVE, CONVERTED)`. 

This library already maps a few different types (mainly `std::string`, `QByteArray` and `QString`):

```c++
GenH5::conversion_t<std::string>;   // == char*
GenH5::conversion_t<QByteArray>;    // == char*
GenH5::conversion_t<QString>;       // == char*
```

The macro should be played at the top of an file, after including `genh5_conversion.h` and before using the conversion system:

```c++
#include "genh5_conversion.h"

struct MyType { ... };

GENH5_DECLARE_CONVERSION_TYPE(MyType, int)

static_assert( 
    std::is_same< GenH5::conversion_t<MyType>, int >, 
    "These types should match!"
);
```

> Default conversions are defined in [src/genh5_conversion/defaults.h](../src/genh5_conversion/defaults.h).
>
> Conversions for the STL and Qt are defined in [src/genh5_conversion/bindings.h](../src/genh5_conversion/bindings.h).

### Buffer Types

As noted earlier some conversion require the buffering of temporary objects. 
For this a similar system is used, in which types are mapped to buffer types.

It can be access using `GenH5::buffer_element_t`. 
Again by default all types are mapped to each other. 

To specify a dedicated buffer type the macro `GENH5_DECLARE_BUFFER_TYPE(TYPE, BUFFER)` can be used. 
Similarly it should be placed at the top of an file, before using the conversion system.

### Conversion

For now we only mapped types to other types. 
No real conversion has took place by now. 

This library provides the `GenH5::convert` method. 
Its generally used internall before writing to a dataset.
There are multiple overloads available for each types that are currently supported. 
However there is no default conversion for every type available.

If a user-defined type can be implicitly converted to its conversion type use the macro `GENH5_DECLARE_IMPLICIT_CONVERSION(TYPE)` to register a new overload:

```c++
GENH5_DECLARE_IMPLICIT_CONVERSION(MyType)
```

However if there is not implicit conversion available use `GENH5_DECLARE_CONVERSION(TYPE_FROM, VALUE_NAME, BUFFER_NAME)`. The macro accepts three arguments:
1) The type to convert from. It may use qualifiers like `const&`.
2) The variable name of the value to convert (e.g. `value`).
3) The variable name of the buffer (e.g. `buffer`). 
Buffer is a container, to which a temporary object may be appended.
The buffer does not have to be used if the conversion does not require any buffering. 

The conversion must then be implemented:

```c++
GENH5_DECLARE_CONVERSION(MyType const&, value, buffer)
{
    Q_UNUSED(buffer)
    return value.getInt();
}
```

The `GenH5::convert` function will be invoked internally by the library. 
When calling the function explicitly its advised to use ADL to allow for custom overloads in the argument dependent lookup:

```c++
using GenH5::convert; // ADL
convert(value, buffer);
```

### Reverse Conversions

After a dataset has been read the "raw" data must be converted to its original/native type.
For this purpose the library implements the method `GenH5::converTo<T>` function which uses an implicit conversion by default.
 
However for some types there may not be any implicit conversions available.

One may use the macro `GENH5_DECLARE_REVERSE_CONVERSION(TYPE_TO, TYPE_FROM, VALUE_NAME)` to implement a dedicated reverse conversion.
The macro accepts three arguments:
1) The orignal type to convert to.
2) The type to convert from.
3) The variable of the value to convert from.

```c++
GENH5_DECLARE_REVERSE_CONVERSION(MyType, int, value)
{
	return MyType::fromInt(value);
}
```

The `GenH5::convertTo` function will be invoked internally by the library. 
When calling the function explicitly its advised to use ADL to allow for custom overloads in the argument dependent lookup:

```c++
using GenH5::convertTo; // ADL
convertTo<MyType>(value);
```

### Generic Conversion

- `GenH5::Array`
- `GenH5::VarLen`
- `GenH5::Comp`

## Datatype Deduction

This library provides the `GenH5::dataType` function, which allows returns the HDF5 datatype associated with the template arguments.

To make this possible a similar system was implemented, which allows to map c++ types to HDF5 datatypes.

This is done using the macro `GENH5_DECLARE_DATATYPE(NATIVE_TYPE, H5_TYPE)`. It accepts two arguments:
1) The c++ type.
2) The target HDF5 type.

```c++
GENH5_DECLARE_DATATYPE(MyType, GenH5::dataType<int>());
```

If the HDF5 type is more complex and cannot be declared inline, use the macro `GENH5_DECLARE_DATATYPE_IMPL(NATIVE_TYPE)`.

The desired datatype can then be implemented in a function-like manner:

```c++
GENH5_DECLARE_DATATYPE_IMPL(MyType)
{
    return GenH5::dataType<int>();
}
```
