/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 * Created on: 21.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_EXCEPTION_H
#define GENH5_EXCEPTION_H

#include <stdexcept>

#ifndef GENH5_NO_EXCEPTION_ID
  #define GENH5_MAKE_EXECEPTION_STR() std::string(__FUNCTION__) + ": "
  #define GENH5_MAKE_EXECEPTION_STR_ID(ID) "GenH5::" ID ": "
#else
  #define GENH5_MAKE_EXECEPTION_STR() "GenH5: "
  #define GENH5_MAKE_EXECEPTION_STR_ID(ID) "GenH5: "
#endif

namespace GenH5
{

/**
 * @brief Exception. Base class for all exceptions of this library
 */
class Exception :
        public std::runtime_error
{
public:

    explicit Exception(std::string const& msg) :
        std::runtime_error{msg}
    { }
};

struct InvalidArgumentError :
        public GenH5::Exception
{
    explicit InvalidArgumentError(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct AttributeException :
        public GenH5::Exception
{
    explicit AttributeException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct DataSetException :
        public GenH5::Exception
{
    explicit DataSetException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct DataTypeException :
        public GenH5::Exception
{
    explicit DataTypeException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct DataSpaceException :
        public GenH5::Exception
{
    explicit DataSpaceException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct FileException :
        public GenH5::Exception
{
    explicit FileException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct GroupException :
        public GenH5::Exception
{
    explicit GroupException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct LocationException :
        public GenH5::Exception
{
    explicit LocationException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct ReferenceException :
        public GenH5::Exception
{
    explicit ReferenceException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct IdComponentException :
        public GenH5::Exception
{
    explicit IdComponentException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

struct PropertyListException :
        public GenH5::Exception
{
    explicit PropertyListException(std::string const& msg) :
        GenH5::Exception{msg}
    {}
};

} // namsepace GenH5

#endif // GENH5_EXCEPTION_H
