#ifndef TOKEN_MAP_CONFIGURABLE_H
#define TOKEN_MAP_CONFIGURABLE_H

#include "export_library.h"
#include <map>
#include <string>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be configured using a variable token map
 */
class TokenMapConfigurable
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
    TokenMapConfigurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~TokenMapConfigurable(void)
    {

    }

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap & /*tokenMap*/) = 0;
};

}

}

#endif
