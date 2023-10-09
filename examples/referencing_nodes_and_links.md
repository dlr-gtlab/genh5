<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# Referencing Nodes and Links

HDF5 objects like datasets, groups and attributes can also be referenced. A reference stores the "index" of the object in the HDF5 file. It is useful for storing an HDF5 object without having to save the internal object path.

### Creating a reference

> **Note:** try-catch blocks were omitted for this example

Creating a reference in GenH5 is rather straightforward. All compatible objects have the method `toReference` which automatically constructs a reference.

```cpp
GenH5::File file("my_file.h5", GenH5::ReadOnly);
GenH5::Group group = file.root().openGroup("path/to/my/group");

GenH5::Reference ref = group.toReference();
```

### Dereferencing

To dereference an object reference simply invoke `toGroup`, `toDataSet` or `toAttribute` on the reference object. Each method requires a HDF5 file object as a parameter: 

```cpp
GenH5::Group group = ref.toGroup(file);
```
