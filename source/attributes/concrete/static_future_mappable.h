#ifndef STATIC_FUTURE_MAPPABLE_H
#define STATIC_FUTURE_MAPPABLE_H

#include <map>
#include <mutex>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of futures, statically shared across all instances of the same
 * type - along with a concrete, static interface to use and manipulate the futures
 */
template<typename T, typename Key, typename Future>
class StaticFutureMappable
{
public:

    /**
     * Constructor
     */
    StaticFutureMappable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticFutureMappable(const StaticFutureMappable<T, Key, Future> &mappable) = delete;

    /**
     * Move constructor
     */
    StaticFutureMappable(StaticFutureMappable<T, Key, Future> &&mappable) = delete;

    /**
     * Destructor
     */
    virtual ~StaticFutureMappable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    StaticFutureMappable<T, Key, Future> &
    operator = (const StaticFutureMappable<T, Key, Future> &mappable) = delete;

    /**
     * Move assignment operator
     */
    StaticFutureMappable<T, Key, Future> &
    operator = (StaticFutureMappable<T, Key, Future> &&mappable) = delete;

    /**
     * Get a reference to this object's map of futures
     */
    inline static std::map<Key, Future> &getFuturesMap(void)
    {
        static std::map<Key, Future> futuresMap;

        return futuresMap;
    }
};

}

}

#endif
