#include <typeindex>

#include "factory_constructible.h"

// using namespace declarations
using namespace functional;

namespace attributes
{

namespace abstract
{

/**
 * Global map of factory maps stored within Any objects and keyed by type
 */
Any &
addFactoryMap(const std::type_info &type,
              Any factoryMap)
{
    if (getFactoryMap(type).empty())
    {
        getFactoryMap(type) = factoryMap;
    }

    return getFactoryMap(type);
}

/**
 * Global map of factory maps stored within Any objects and keyed by type
 */
Any &
getFactoryMap(const std::type_info &type)
{
    static std::map<std::type_index,
                    functional::Any> factoryMap; // map of factories stored as any's

    return factoryMap[type];
}

}

}

