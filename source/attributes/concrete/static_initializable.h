#ifndef STATIC_INITIALIZABLE_H
#define STATIC_INITIALIZABLE_H

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that can statically perform initialization
 */
template<typename T>
class StaticInitializable
{
public:

    /**
     * Constructor
     */
    StaticInitializable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticInitializable(void)
    {

    }

    /**
     * Static initialization function
     */
    inline static bool staticInitialize(void)
    {
        return T::staticInitialize();
    }
};

}

}

#endif
