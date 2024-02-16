<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# Reading and writing data from and to a dataset

See [file handling and interal path traversal](file_handling_and_traversal.md) for more details on how to open and create a dataset or attribute.

There a multiple ways to read and write data from and to a dataset. 
In the following, different scenarios will be used to illustrate how to use the API. 
Most methods will apply to Attributes as well.

See [creating data object](creating_data.md) for more details on how to use the Data API. 

> **Note:** try-catch blocks were omitted for the following examples.

## Scalar Data (0D)

> **Note:** The root node can be accessed using `GenH5::File::root()`.

One may use the `GenH5::Data0D` class to write scalar data (i.e. a single element). This class will provide additional safty checks and convenience and should be favored most of the time.

- Example usage for plain data:

    ```cpp
    GenH5::Data0D<int> data = 42;
    // use createAttribute for creating an attribute
    GenH5::DataSet dset = root.createDataSet("my_dset_0d", 
                                             data.dataType(), // datatype and dataspace may be deduced automatically
                                             data.dataSpace());
    dset.write(data);
    ```

- Writing compound data (here: `int` and `std::string`):

    ```cpp
    GenH5::Data0D<int, std::string> data{
        42, std::string("hello world")
    };
    GenH5::DataSet dset = root.createDataSet("my_comp_dset_0d", 
                                             data.dataType(), 
                                             data.dataSpace());
    dset.write(data);
    ```

- Writing a string with fixed length. By default all strings will be interpreted as an variable length string. To enforce the usage of a fixed string datatype use `GenH5::FixedString0D`:

    ```cpp 
    GenH5::FixedString0D fixedstr = std::string("hello world"); 
    // fixedstr.stringSize() == 11 (does not include trailing \0)
    
    GenH5::DataSet dset = root.createDataSet("my_fixedstr_dset_0d",
                                             fixedstr.dataType(), 
                                             fixedstr.dataSpace());
    dset.write(fixedstr);
    ```

When reading Scalar data simply invoke `read()` on the dataset/attribute instead.

### Convenience API:

GenH5 also provides multiple functions to read a dataset or attribute in a single function call.

- This will overwrite or create the dataset/attribute (if it does not exist) and write the data to it:

    ```cpp
    // use writeAttribute0D for creating an attribute
    root.writeDataSet0D("my_dset_0d", 42); // datatype deduced as "int"
    ```

- The same can be used for compound data using the typedef `GenH5::Comp` or the method `GenH5::makeComp0D()`:

    ```cpp
    root.writeDataSet0D(
        "my_comp_dset_0d",
        GenH5::makeComp0D(-1.23, QString{"this is a compound 0D"}) // will be deduced as "double" and "QString"
    );
    ```
    same as
    ```cpp
    root.writeDataSet0D(
        "my_comp_dset_0d",
        GenH5::Comp<double, QString>(-1.23, "this is a compound 0D")
    );
    ```

- Writing a fixed string:
    
    ```cpp
    root.writeDataSet0D(
        "my_fixedstr_dset_0d", 
        // will be deduced as a fixed string of length 12 (includes trailing \0)
        "hello world" 
    );
    ```
    alternatively
    ```cpp
    root.writeDataSet0D(
        "my_fixedstr_dset_0d", 
        // will be deduced as a fixed string of length 11 (not including trailing \0)
        GenH5::makeFixedStr(QString{"hello world"}) 
    );
    ```

<details>
<summary>Wrting GenH5::Data0D directly</summary>

When using `GenH5::Data0D` or `GenH5::FixedString0D` directly, use the `writeDataSet()` or `writeAttribute()` instead (without "0D" in the name):

```cpp
// use writeAttribute for creating an attribute
GenH5::Data0D<int> data = 42;
root.writeDataSet("my_dset_0d", data); // same as before
```
for compounds:
```cpp
GenH5::Data0D<int, std::string> data{
    42, std::string("hello world")
};
root.writeDataSet("my_comp_dset_0d", data); // same as before
```
for fixed strings:
```cpp
GenH5::FixedString0D fixedstr = std::string("hello world")
root.writeDataSet("my_fixedstr_dset_0d", fixedstr); // same as before
```

</details>

<details>
<summary>Reading 0D data</summary>

To read the data use `readDataSet(0D)` or `readAttribute(0D)` instead. It requires the return type as template argument. It will always return a type of `GenH5::Data(0D)` or `GenH5::CompData(0D)`:

- Reading a single integer:

    ```cpp
    // use readAttribute0D for reading an attribute
    GenH5::Data0D<int> data = root.readDataSet0D<int>("my_dset_0d");
    ```

- Reading a compound type:
    ```cpp
    GenH5::Data0D<int, std::string> data = root.readDataSet0D<int, std::string>("my_comp_dset_0d");
    ```
    or alternatively
    ```cpp
    auto data = root.readDataSet0D<GenH5::Comp<int, std::string>>("my_comp_dset_0d"); // "auto" is encouraged
    ```

- Reading a fixed string:
    ```cpp
    auto fixedstr = root.readDataSet0D<GenH5::FixedString0D>("my_fixedstr_dset_0d");
    ```

</details>

## 1D Data

When reading or writing 1D (or higher ranked) data use the `GenH5::Data` typedef. Its usage is near identical to reading/writing scalar data with `GenH5::Data0D`.

- Example usage for plain data:

    ```cpp
    GenH5::Data<int> data = { 1, 2, 3, 4 }; // accepts most container types (std::vector, QVector, etc.)
    // use createAttribute for creating an attribute
    GenH5::DataSet dset = root.createDataSet("my_dset", 
                                             data.dataType(), // datatype and dataspace may be deduced automatically
                                             data.dataSpace());
    dset.write(data);
    ```

- Writing compound data (here: `double` and `QString`):

    ```cpp
    // data with single element
    GenH5::Data<double, QString> data{ 
        42, QString("my fancy string") 
    };
    // OR 
    // multiple elements 
    GenH5::Data<double, QString> data{
        QVector<double>{...}, QStringList{...} // all arguments must have the same size!
    };
    GenH5::DataSet dset = root.createDataSet("my_comp_dset", 
                                             data.dataType(), 
                                             data.dataSpace());
    dset.write(data);
    ```

When reading 1D or higher ranked data simply invoke `read()` on the dataset/attribute instead.

### Convenience API:

Once again, GenH5 provides multiple functions to read a dataset or attribute in a single function call.

- This will overwrite or create the dataset/attribute (if it does not exist) and write the data to it:

    ```cpp
    // use writeAttribute for creating an attribute
    QVector<int> myInts{1, 2, 3, 4}
    root.writeDataSet("my_dset", myInts); // deduced as 1D data of "int"
    ```

- The same can be used for compound data using the typedef `GenH5::makeComp()` or `GenH5::makeCompData()`:

    ```cpp
    QVector<double> myDoubles{1, 2, 3, 4}
    QStringList myStrs{"this","is", "a", "compound"}
    file.root().writeDataSet(
        "my_comp_dset",
        // will be deduced as 1D data of "double" and "QString"
        GenH5::makeCompData(myDoubles, myStrs)
    );
    ```

- Using `GenH5::Data` directly:
    ```cpp
    // use writeAttribute for creating an attribute
    GenH5::Data<int> data = myInts;
    root.writeDataSet("my_dset", data); // same as before
    ```
    for compounds:
    ```cpp
    GenH5::Data<double, QString> data{ myDoubles, myStrs };
    root.writeDataSet("my_comp_dset", data); // same as before
    ```

<details>
<summary>Reading 1D or higher ranked data</summary>

To read the data use `readDataSet()` or `readAttribute()` instead. It requires the return type as template argument. It will always return a type of `GenH5::Data` or `GenH5::CompData`.

> **Note:** One may also use `readDataSet0D()` or `readAttribute0D()`; however, this will only read the first values.

- Reading a single integer:

    ```cpp
    // use readAttribute for reading an attribute
    GenH5::Data<int> data = root.readDataSet<int>("my_dset");
    ```

- Reading a compound type:
    ```cpp
    GenH5::Data<double, QString> data = root.readDataSet0D<double, QString>("my_comp_dset_0d");
    ```
    or alternatively
    ```cpp
    auto data = root.readDataSet0D<GenH5::Comp<double, QString>>("my_comp_dset_0d"); // "auto" is encouraged
    ```
</details>

## Multidimensional data (2D+):

As noted before, `GenH5::Data` and `GenH5::CompData` will create a 1D dataspace by default. However, these types can be used for creating multi dimensional data as well. There are two ways of archiving this:

- One way of creating a 2D Dataset (for example) is by passing in a `GenH5::DataSpace` object with the desired dimensions when using `createDataSet()` or `createAttribute()`:

    ```cpp
    GenH5::Data<int> data{ 1, 2, 3, 4, 5, 6, 7, 8 };
    // use createAttribute for creating an attribute
    GenH5::DataSet dset = root.createDataSet("my_2d_dset", 
                                             data.dataType(), 
                                             GenH5::DataSpace{4, 2}); // custom dataspace
    dset.write(data);
    ```

- Alternatively, one can use the `setDimensions()` method on the `GenH5::Data` and `GenH5::CompData` objects (see [creating data object](creating_data.md) for more details on how to create multidimensional data). One may then access the multidimensional dataspace.

    > **Note:** Calling `setDimensions()` will not truncate nor extend the data. It is simply an optional property for "displaying" the data.

    ```cpp
    GenH5::Data<int> data{ 1, 2, 3, 4, 5, 6, 7, 8 };
    data.setDimensions({4, 2});
    GenH5::DataSet dset = root.createDataSet("my_2d_dset", 
                                             data.dataType(), 
                                             data.dataSpace());
    dset.write(data);
    ```

    The data object can then also be used with the convenience API using `writeDataSet()` or `writeAttribute()`:

    ```cpp
    GenH5::Data<int> data{ 1, 2, 3, 4, 5, 6, 7, 8 };
    data.setDimensions({4, 2});

    // use writeAttribute for creating an attribute
    GenH5::DataSet dset = root.writeDataSet("my_2d_dset", data);
    ```

<details>
<summary>Reading multidimensional data</summary>

Reading a higher dimensional dataset will automatically set dimensions property of `GenH5::Data`, which can be accessed using the `dimensions()` function. Reading works identically to reading a 1D dataset otherwise:

```cpp
// use writeAttribute for creating an attribute
GenH5::Data<int> data = root.readDataSet<int>("my_2d_dset");
auto dims = data.dimensions(); // dims = {4, 2}
```

</details>


## Accessing individual compound members

Sometimes it is necessary/useful to access individual compound members instead of the whole dataset. Say we have a compound type with three members:
 1. Member `A` with an `int` datatype
 2. Member `B` with an `std::string` datatype
 3. Member `C` with an `VarLen<int>` datatype

> **Note:** This is currently not possible using the convenience API

- To only read Member `C`, we can use the `GeH5::CompData` typedef and call the `setTypeNames()` function to set the name to `"C"` and read the dataset that way:

    ```cpp
    auto dset = root.openDataSet("my_complex_dset");

    GenH5::Data<GenH5::VarLen<int>> data;
    data.setTypeNames({"C"}); 
    dset.read(data);

    auto c = data.getValues<0>();
    ```

- This also works for multiple members, say we want to read member `C` and then member `B`:

    ```cpp
    auto dset = root.openDataSet("my_complex_dset");

    GenH5::Data<GenH5::VarLen<int>, std::string> data;
    data.setTypeNames({"C", "B"}); 
    dset.read(data);

    auto c = data.getValues<0>();
    auto b = data.getValues<1>();
    ```

## Alternative to reading Array-Data

Sometimes one may wants to read array data but does not know exactly how long the datatype is at compile time. Therefore we cannot use the `GenH5::Array` typedef.

However the `GenH5::Data` has a convenience feature built into the `resize()` method, which is used by the libary to reserve enough memory for reading data.

Say we want to read array data containing `int`s. We can then use `GenH5::Data<int>` to read the array data as follows:

```cpp
auto dset = root.openDataSet("my_array_dset");
GenH5::Data<int> data;

// of course we have to make sure we are actually dealing with int array data...
assert(dset.dataType().isArray());
assert(dset.dataType().super() == GenH5::dataType<int>());

// read the data is if it were the dataset type
dset.read(data, dset.dataType()); 
// OR less error prone
dset.read(data, GenH5::DataTpye::array(GenH5::datatype<int>(),
                                       dset.dataType().arrayDimensions()));
```

Note that we have to pass the desired datatype to the `read()` method. We now have a single 1D vector of `int`s. 

Further, we can now make use of the `split()` function of the data object to split the data into a nested vector:

```cpp
// if we know how long the array data is
GenH5::Vector<GenH5::Vector<int>> data2D = data.split(3);
// OR summing up the array dimensions
GenH5::Vector<GenH5::Vector<int>> data2D = data.split(GenH5::prod(dset.datatype().arrayDimensions()));
```

## Usage of void*

When reading or writing data to a dataset or attribute an additional method can be used, that accepts a plain `void (const)*`. 

It can be used like this:
```cpp
GenH5::Group const& root = file.root();
// use createAttribute for creating an attribute
GenH5::DataSet dset = root.createDataSet("my_dset_0d", GenH5::dataType<int>(), GenH5::Scalar);

int value = 42;
dset.write(&value);
```

However, since these methods accepts a `void*` special care is required to pass the value correctly. It only works if the data that is pointed to is contiguous (e.g. a plain `int`, `double`, `char*` or arrays like `float[42]`). It should not be used with user defined types like `std::string` etc.
