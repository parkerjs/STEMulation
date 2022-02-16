#ifndef STATIC_MUTEX_MAPPABLE_H
#define STATIC_MUTEX_MAPPABLE_H

#include "index_of.h"
#include <map>
#include <tuple>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of mutex locks, statically shared across all instances of the
 * same type, for thread synchronization - along with a concrete, static interface to use and manipulate the
 * mutexes
 */
template<typename T, typename Key, typename Mutex>
class StaticMutexMappable
{
public:

    /**
     * Constructor
     */
    StaticMutexMappable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticMutexMappable(const StaticMutexMappable<T, Key, Mutex> &mappable) = delete;

    /**
     * Move constructor
     */
    StaticMutexMappable(StaticMutexMappable<T, Key, Mutex> &&mappable) = delete;

    /**
     * Destructor
     */
    virtual ~StaticMutexMappable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    StaticMutexMappable<T, Key, Mutex> &
    operator = (const StaticMutexMappable<T, Key, Mutex> &mappable) = delete;

    /**
     * Move assignment operator
     */
    StaticMutexMappable<T, Key, Mutex> &
    operator = (StaticMutexMappable<T, Key, Mutex> &&mappable) = delete;

    /**
     * Get a reference to this object's mutex map
     */
    template<typename U = Key>
    inline static std::map<U, Mutex> &getMutexMap(void)
    {
        static std::map<U, Mutex> mutexMap;

        return mutexMap;
    }
};

/**
 * This specialization implements a base collector class and serves to disambiguate calls to the getMutexMap()
 * function in cases where multiple key-
 *                             mutex map pairings
 */
template<typename T, typename ... Keys, typename ... Mutexes>
class StaticMutexMappable<T, std::tuple<Keys ...>, std::tuple<Mutexes ...>>
: public StaticMutexMappable<T, Keys, Mutexes> ...
{
public:

    /**
     * Typedef declarations
     */
    typedef std::tuple<Keys ...> tKeys;
    typedef std::tuple<Mutexes ...> tMutexes;

    /**
     * Get a reference to this object's mutex map
     */
    template<typename Key>
    inline static auto &getMutexMap(void)
    {
        typedef typename std::tuple_element<traits::helpers::index_of<Key, tKeys>::value, tMutexes>::type Mutex;

        return StaticMutexMappable<T, Key, Mutex>::template getMutexMap<Key>();
    }
};

}

}

#endif
