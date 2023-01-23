# Creating and Reading Datasets (WIP)

Creating groups, datasets and attributes is rather straightforward. 

One you have opened the file access the root group using the function `GenH5::File::root`.
The root group can then be used to create subgroups, datasets and attributes.

> **Note:** try-catch blocks were omitted for this example

### Creating and accessing groups

To create a group call `createGroup` on a *group* object (e.g. the root group)

```cpp
GenH5::Group const& root = file.root();
GenH5::Group subgroup = root.createGroup("my_group"); // will create the group if it does not exist
```

If the group provided already exsits it will be opened instead.

If you strictly want to open a group use `openGroup` instead.

```cpp
GenH5::Group const& root = file.root();
GenH5::Group subgroup = root.openGroup("my_group"); // will fail if my_group does not exist
```

### Creating and accesing datasets



