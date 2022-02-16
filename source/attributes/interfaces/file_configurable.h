#ifndef FILE_CONFIGURABLE_H
#define FILE_CONFIGURABLE_H

#include "export_library.h"
#include <string>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be configured using input file or input string
 */
class FileConfigurable
{
public:

    /**
     * Constructor
     */
    FileConfigurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~FileConfigurable(void)
    {

    }

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(const std::string & /*fileOrString*/) = 0;
};

}

}

#endif
