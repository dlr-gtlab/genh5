# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Fixed
- Fixed invalid cmake config file casing, making them not found under linux - #64

## [3.0.1] - 2023-05-04
### Added
- methods for accessing parent node (`Node::parent`) and node object of attribute (`Attribute::nodeInfo`) - #53

### Changed
- `Location::name` will now return `/` for the root group instead of an empty string

## [3.0.0] - 2023-02-02
### Added
- `IdComponent` for managing the lifetime of an object id handle
- `IdComponentException`
- Implemented a basic logging interface for forwarding error messages to custom output sinks - #12

### Changed
- Internal refactoring, due to removed HDF5 C++ API dependencie
- Predefined DataTypes (`Bool`, `Double` etc.) and DataSpaces (`Scalar`, `Null`) are now `static const&`
- `Location::file` no longer returns a shared ptr but a local `File` object. Refactored constructors of derived classes - #45
- Better Exceptions strings - #50

### Removed
- `Object::type` use `GenH5::classType` instead
- `Data::deserialize`, `Data::deserializeIdx`, `Data::c`
- `AbstractData`
- `File::isValidH5File`
- `Node::versionAttrName`, `Node::createVersionAttribute`
- `Group::createDataset`, `Group::openDataset`
- `DataSet::properties`
- `DataSetCProperties::setCompression`
- `toH5` methods and constructors, that use the HDF5 C++ API

## [2.2.4] - 2023-01-16
### Added
- Method for deleting a node recursively

### Fixed 
- Fixed `Group::deleteLink` and `DataSet::deleteLink` for nested nodes - #48

## [2.2.3] - 2023-01-13
### Fixed 
- Fixed a bug in `Data::resize` method returning false negatives - #47

## [2.2.2] - 2023-01-12
### Fixed
- Fixed some edge cases in which reading data would proceed, eventhough the data was not properly resized and had too few elements - #41
- A single compound array type like `Array<int, 5>` can now be read using its super type (here `int`) - #41
- explicitly deleted usage of static FixedString0D::dataType method

### Changed
- The data dimensions of `CommonData` will be set according to the dataset that is beeing read (via the `resize` method) - #44
- `Group::readDataSet(0D)` is now `const`

### Deprecated
- `Group::createDataset` use `Group::createDataSet` instead - #46
- `Group::openDataset` use `Group::openDataSet` instead - #46

## [2.2.1] - 2022-12-05
### Added
- Methods for reading 0D Data from a dataset or attribute (see `Node::readAttribute0D` and `Group::readDataSet0D`)
- Added a few getter methods for checking if a datatype is an int, float or string

### Fixed
- `DataType::array` and `DataType::varlen` will no longer throw `H5::Exception` - #38
- `operator==` for `DataType` will now check recursively if the datatypes are equal. Fixed multiple false positves. - #39
- Read/Write methods of `AbstractDataSet` will now use the correct type when reading or writing data. - #39
- `dataType<Array<char, N>()` is now a fixed string datatpye of length N and the same as `dataType<char[N]>()`

### Deprecated
- `toH5` methods that return H5Cpp classes

## [2.2.0] - 2022-11-18
### Changed
- `Data::setTypeNames` will be deactivated for non-compound types - #35

### Added
- `FixedString0D` for fixed strings - #34
- Added static methods for accessing subrelease in `Version` struct

## [2.1.0] - 2022-10-27
### Changed
- `Location::type` was moved to `Object::type`
- Updated type traits

### Added
- Added support for cmake
- Added support for the conan package manager
- `Data0D` and `CompData0D` for scalar data
- `makeArrays` and `makeVarLens` utils functions. Implemented `unpack` overloads accordingly - #25
- `Data::split` method for reading array types more easily - #25
- Getters for deflate compression - #29
- `idx` utils function and overloads for `Data::operator[]`, `Data::at` and `Data::value` to access multidimensional data more easily. 
   The desired data dimensions must be set prior using `Data::setDimensions`. - #23
- `CompData0D::setValue` and `Data::setValue` methods - #31
- Methods for iterating over and finding child nodes and attributes - #24
- High level methods for writing and reading datasets and attributes in one call - #27

### Deprecated 
- `Data::deserialize` and `Data::deserializeIdx`, use `Data::value(s)`, `Data::unpack` or `CompData::getValue(s)` instead. - #23
- `Data::c`, use `Data::raw` instead
- `AbstractData` use `CommonData` or `details::AbstractData` instead
- `Node::versionAttrName` use `Node::versionAttributeName` instead
- `Node::createVersionAttribute` use `Node::writeVersionAttribute` instead
- `File::isValidH5File`

### Fixed 
- `Attribute::doRead` will now throw a `GenH5::Exception`

## [2.0.0 - 2022-06-28]
### Changed
- Changed namespace to `GenH5` - #14
- Wrapper now uses exceptions - #11

### Added
- `GTH5_VERSION_CHECK` macro and version struct
- support for additional datatypes (fixed sized arrays, predefined types, varlen and compound types) - #7
- support for partial read/write operations (i.e. hyperslap selections) - #9 

### Removed 
- `DataSpace::sum` use `DataSpace::size` or `GenH5::prod` instead
- Old `Data` and `AbstractData` structs. Use `GenH5::Data` or `GenH5::CompData` instead - #7
- methods that use `QString`. Use `toUtf8()` instead - #14

## [1.0.3] - 2022-04-13
### Changed
- moved to c++14 - #4
- updated prefix naming sceme to GtH5 - #5
- improved const correctness, minor interface changes - #4

### Added
- added versioning - #4

## [1.0.2] - 2021-11-17
### Fixed
- Fixed error when inserting members in compound datatype when in debug mode - #2

## [1.0.1] - 2021-11-10
### Fixed
- updated paths to the hdf5 debug libraries

## [1.0.0] - 2021-11-08
### Added
- initial release
- Creating, accessing and deleting files, groups, datasets and attributes
- Chunked datasets and compression
- Flexible template-based datatype interface for simple and comlex compound types (using `GtH5Data<...>`)
- Simple interface for dataspaces
- Simple interface for (de-) referencing nodes and attributes
