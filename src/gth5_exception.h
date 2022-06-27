/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 21.06.2022
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef GTH5_EXCEPTION_H
#define GTH5_EXCEPTION_H

#include <stdexcept>

namespace GtH5
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
        public GtH5::Exception
{
    explicit InvalidArgumentError(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct AttributeException :
        public GtH5::Exception
{
    explicit AttributeException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct DataSetException :
        public GtH5::Exception
{
    explicit DataSetException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct DataTypeException :
        public GtH5::Exception
{
    explicit DataTypeException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct DataSpaceException :
        public GtH5::Exception
{
    explicit DataSpaceException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct FileException :
        public GtH5::Exception
{
    explicit FileException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct GroupException :
        public GtH5::Exception
{
    explicit GroupException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct LocationException :
        public GtH5::Exception
{
    explicit LocationException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

struct ReferenceException :
        public GtH5::Exception
{
    explicit ReferenceException(char const* msg) :
        GtH5::Exception{msg}
    {}
};

} // namsepace GtH5

#endif // GTH5_EXCEPTION_H
