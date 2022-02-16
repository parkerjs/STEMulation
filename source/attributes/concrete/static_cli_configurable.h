#ifndef STATIC_CLI_CONFIGURABLE_H
#define STATIC_CLI_CONFIGURABLE_H

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that can be statically configured using
 * command-line arguments
 */
template<typename T>
class StaticCLI_Configurable
{
public:

    /**
     * Constructor
     */
    StaticCLI_Configurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticCLI_Configurable(void)
    {

    }

    /**
     * Configure function (using command-line arguments)
     */
    inline static bool staticConfigure(int argc, char **argv)
    {
        return T::staticConfigure(argc, argv);
    }
};

}

}

#endif
