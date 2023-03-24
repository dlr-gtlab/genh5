# Find child nodes and attributes

The structure of an HDF5 file can easily be traversed. 

The following example shows different ways of accessing child nodes (groups or datasets) and their attributes.

## Example

> **Note:** try-catch blocks were omitted for this example

### Creating a sample file structure

For this example we consider the following file structure:

```
/
│
└───A/
│   │   attr: GENH5_VERSION
│   │   attr: my_value
│   │
│   └───my_dset
│   │
│   └───data/
│       └───1
│       │
│       └───2
└───B/
    │   attr: GENH5_VERSION
    │   attr: my_attr
    │
    └───empty
    │
    └───my_sub_group/
``` 

We can create this using the following snippet:

```cpp
GenH5::File file{"my_file.h5", GenH5::Create};

// group "A"
GenH5::Group a = file.root().createGroup("A");
a.createVersionAttribute();
a.createAttribute("my_value", GenH5::dataType<double>(),
                  GenH5::DataSpace::Scalar);
a.createDataset("my_dset", GenH5::dataType<int>(),
                GenH5::DataSpace::Scalar);

// group "A/data"
GenH5::Group data = a.createGroup("data");
data.createDataset("1", GenH5::dataType<int>(),
                   GenH5::DataSpace::linear(10)).
        createVersionAttribute();
data.createDataset("2", GenH5::dataType<double>(),
                   GenH5::DataSpace{5, 10}).
        createVersionAttribute();

// group "B"
GenH5::Group b = file.root().createGroup("B");
b.createVersionAttribute();
b.createAttribute("my_attr", GenH5::dataType<QString>(),
                  GenH5::DataSpace::Scalar);
b.createDataset("empty", GenH5::dataType<char>(),
                GenH5::DataSpace::Null);
				
// group "B/my_sub_group"
b.createGroup("my_sub_group");
```

### Find child nodes

Use `Group::findChildNodes` to get a list of all child nodes (empty if group has no children).

The method returns a list of `NodeInfo` structs, which may either belong to another group or a dataset. 
Using the `toGroup` or `toDataset` a new node may be created. 
One may check if its "save" to access a node as a group or dataset using the `NodeInfo::isGroup` or `NodeInfo::isDataSet` methods otherwise the method may throw. 
Alternatively `toNode` can be used to create a `Node` object on the heap. 

```cpp
// find all nodes 
GenH5::Vector<NodeInfo> allNodes = file.root().findChildNodes(GenH5::FindRecursive);

// find direct nodes only
GenH5::Vector<NodeInfo> directNodes = file.root().findChildNodes(GenH5::FindDirectOnly);

// find all groups
GenH5::Vector<NodeInfo> onlyGroups = file.root().findChildNodes(GenH5::FindRecursive,
                                                                GenH5::FilterGroups);

// find dsets relative to group A
GenH5::Vector<NodeInfo> aDsets = a.findChildNodes(GenH5::FindRecursive,
                                                  GenH5::FilterDataSets);
```

A filter may be used to find nodes of a certain type. 
In that case it is guranteed that the `NodeInfo` struct does belong to the node type supplied (e.g. when using `FilterDataSets` all info structs refer to datasets)

```cpp
for (NodeInfo const& info : aDsets)
{
    // in this case info.isDataSet() will always return true here
    auto dset = info.toDataSet(a);
    ... // do something
}
```

### Find attributes

Use `Node::findAttributes` to get a list of all attributes belonging to the node, on which the method was invoked on (empty if node has no attributes).

The method returns a list of `AttributeInfo` structs. 
Using the `toAttribute` method an attribute object may be created.


```cpp
GenH5::Vector<AttributeInfo> bAttrs = b.findAttributes();

for (AttributeInfo const& info : bAttrs)
{
    auto attr = info.toAttribute(b);
    ... // do something
}
```

### Iterating over nodes and attributes

Additionally one can use the methods `Group::iterateChildNodes` or `Node::iterateAttributes` to invoke a method on each node or attribute respectively.

```cpp
// prints the absolute path of all child nodes
b.iterateChildNodes([](GenH5::Group& parent, GenH5::NodeInfo const& info) -> herr_t {
    qDebug() << parent.path() + "/" + info.path;
    return 0; // success
}, GenH5::FindRecursive);
```

> **Note:** Do not throw exceptions in the iteration functor! The HDF5 C-library may not be able to cleanup afterwards resulting in memory leaks.
