#ifndef STATIC_SETUPABLE_H
#define STATIC_SETUPABLE_H

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that can statically perform setup
 */
template<typename T>
class StaticSetupable
{
public:

    /**
     * Constructor
     */
    StaticSetupable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticSetupable(void)
    {

    }

    /**
     * Static setup function
     */
    inline static bool staticSetup(void)
    {
        return T::staticSetup();
    }
};

}

}

#endif
