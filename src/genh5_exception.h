/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GENH5_EXCEPTION_H
#define GENH5_EXCEPTION_H

#include <stdexcept>

namespace GenH5
{

/**
 * @brief Exception. Base class for all exceptions of this library
 */
struct Exception :
        public std::runtime_error
{
    explicit Exception(char const* msg) :
        std::runtime_error{msg}
    {}
};

struct InvalidArgumentError :
        public GenH5::Exception
{
    explicit InvalidArgumentError(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct AttributeException :
        public GenH5::Exception
{
    explicit AttributeException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct DataSetException :
        public GenH5::Exception
{
    explicit DataSetException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct DataTypeException :
        public GenH5::Exception
{
    explicit DataTypeException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct DataSpaceException :
        public GenH5::Exception
{
    explicit DataSpaceException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct FileException :
        public GenH5::Exception
{
    explicit FileException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct GroupException :
        public GenH5::Exception
{
    explicit GroupException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct LocationException :
        public GenH5::Exception
{
    explicit LocationException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

struct ReferenceException :
        public GenH5::Exception
{
    explicit ReferenceException(char const* msg) :
        GenH5::Exception{msg}
    {}
};

} // namsepace GenH5

#endif // GENH5_EXCEPTION_H
