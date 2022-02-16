#ifndef STATIC_CLI_ARGUMENT_PROCESSABLE_H
#define STATIC_CLI_ARGUMENT_PROCESSABLE_H

// forward declarations
namespace utilities { class CLI_ArgumentProcessor; }

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that process arguments via command line interface
 * (CLI)
 */
template<typename T>
class StaticCLI_ArgumentProcessable
{
public:

    /**
     * Constructor
     */
    StaticCLI_ArgumentProcessable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticCLI_ArgumentProcessable(const StaticCLI_ArgumentProcessable<T> &processable) = delete;

    /**
     * Move constructor
     */
    StaticCLI_ArgumentProcessable(StaticCLI_ArgumentProcessable<T> &&processable) = delete;

    /**
     * Destructor
     */
    virtual ~StaticCLI_ArgumentProcessable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    StaticCLI_ArgumentProcessable<T> &
    operator = (const StaticCLI_ArgumentProcessable<T> &processable) = delete;

    /**
     * Move assignment operator
     */
    StaticCLI_ArgumentProcessable<T> &
    operator = (StaticCLI_ArgumentProcessable<T> &&processable) = delete;

    /**
     * Get a pointer to this object's CLI argument processor
     */
    inline static utilities::CLI_ArgumentProcessor *getStaticCLI_ArgumentProcessor(void)
    {
        return T::getStaticCLI_ArgumentProcessor();
    }

    /**
     * Set a pointer to this object's CLI argument processor
     */
    inline static void setStaticCLI_ArgumentProcessor(utilities::CLI_ArgumentProcessor *pCLI_ArgumentProcessor)
    {
        T::setStaticCLI_ArgumentProcessor(pCLI_ArgumentProcessor);
    }
};

}

}

#endif
