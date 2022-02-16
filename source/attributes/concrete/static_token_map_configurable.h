#ifndef STATIC_TOKEN_MAP_CONFIGURABLE_H
#define STATIC_TOKEN_MAP_CONFIGURABLE_H

#include <map>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that can be statically configured using a
 * variable token map
 */
template<typename T>
class StaticTokenMapConfigurable
{
public:

    /**
     * Typedef declarations
     */
    typedef std::map<std::string, std::string> tTokenMap;
    typedef std::map<std::string, tTokenMap> tTokenMaps;

    /**
     * Constructor
     */
    StaticTokenMapConfigurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticTokenMapConfigurable(void)
    {

    }

    /**
     * Configure function (using a variable token map)
     */
    inline static bool staticConfigure(tTokenMap &tokenMap)
    {
        return T::staticConfigure(tokenMap);
    }
};

}

}

#endif
