# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Changed 
- Changed namespace to `GenH5` - #14
- Wrapper now uses exceptions - #11

### Added
- `GTH5_VERSION_CHECK` macro
- support for additional datatypes (fixed sized arrays, predefined types, varlen and compound types) - #7
- support for partial read/write operations (i.e. hyperslap selections) - #9 

### Deprecated 
- `GtH5DataSpace::sum` use `size` or `GtH5::noe` instead
- `GtH5Data` and `GtH5AbstractData` use `GtH5::Data` or `GtH5::CompData` instead - #7
- methods that use `QString`. Use `toUtf8()` instead - #14

### Removed

## [1.0.3] - 2022-04-13
### Changed 
- moved to c++14 - #4
- updated prefix naming sceme to gth5 - #5
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
