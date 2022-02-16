#ifndef STATIC_FILE_CONFIGURABLE_H
#define STATIC_FILE_CONFIGURABLE_H

#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that can be statically configured using input
 * file or input string
 */
template<typename T>
class StaticFileConfigurable
{
public:

    /**
     * Constructor
     */
    StaticFileConfigurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticFileConfigurable(void)
    {

    }

    /**
     * Configure function (using input file or input string)
     */
    inline static bool staticConfigure(const std::string &fileOrString)
    {
        return T::staticConfigure(fileOrString);
    }
};

}

}

#endif
