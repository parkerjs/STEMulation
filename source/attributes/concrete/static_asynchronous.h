#ifndef STATIC_ASYNCHRONOUS_H
#define STATIC_ASYNCHRONOUS_H

#include "invoke.h"
#include <map>
#include <mutex>

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that use futures to obtain the results of
 * simultaneously executing threads across instances of the same type
 */
template<typename T>
class StaticAsynchronous
{
protected:

    /**
     * Constructor
     */
    StaticAsynchronous(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticAsynchronous(const StaticAsynchronous<T> &asynchronous) = delete;

    /**
     * Move constructor
     */
    StaticAsynchronous(StaticAsynchronous<T> &&asynchronous) = delete;

public:

    /**
     * Destructor
     */
    virtual ~StaticAsynchronous(void)
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    StaticAsynchronous<T> &operator = (const StaticAsynchronous<T> &asynchronous) = delete;

    /**
     * Move assignment operator
     */
    StaticAsynchronous<T> &operator = (StaticAsynchronous<T> &&asynchronous) = delete;

public:

    /**
     * Add a future; returns true upon success
     * @param key    the key associated with the given future object
     * @param future an r-value reference to a future object
     */
    template<typename Key, typename Future>
    inline static void addFuture(Key &&key, Future &&future)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &futuresMap = T::getFuturesMap();
        auto &&itFutureEntry = futuresMap.find(std::forward<Key>(key));
        bool bSuccess = (itFutureEntry == futuresMap.end());
        if (bSuccess)
            futuresMap.emplace(key, std::forward<Future>(future));

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Get a pointer to a future specified by key
     * @param  key the associated future object to be retrieved
     * @return     a non-null pointer to the associated future, if found
     */
    template<typename Key>
    inline static auto *getFuture(Key &&key)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &&futuresMap = T::getFuturesMap();
        auto &&itFutureEntry = futuresMap.find(std::forward<Key>(key));
        typedef typename std::decay<decltype(itFutureEntry->second)>::type Future;
        Future *pFuture = nullptr;
        if (itFutureEntry == futuresMap.cend())
        {
            auto &&pair = futuresMap.emplace(std::piecewise_construct, std::make_tuple(std::forward<Key>(key)),
                                             std::make_tuple());
            if (pair.second)
                itFutureEntry = pair.first;
        }

        if (itFutureEntry != futuresMap.cend())
            pFuture = &itFutureEntry->second;

        mutex.unlock();

        return pFuture;
    }

    /**
     * Remove a future
     * @param key the key associated with the future object to be removed
     */
    template<typename Key>
    inline static bool removeFuture(Key &&key)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &&futuresMap = T::getFuturesMap();
        auto &&itFutureEntry = futuresMap.find(std::forward<Key>(key));
        bool bSuccess = (itFutureEntry != futuresMap.cend());
        if (bSuccess)
            futuresMap.erase(itFutureEntry);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set a future object specified by key
     * @param key    the key associated with the future object
     * @param future an r-value reference to a future object
     */
    template<typename Key, typename Future>
    inline static void setFuture(Key &&key, Future &&future)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &&futuresMap = T::getFuturesMap();
        futuresMap[std::forward<Key>(key)] = std::move(future);
        mutex.unlock();
    }

    /**
     * Set this object's map of future objects
     * @param futuresMap a reference to a map of future objects
     */
    template<typename Key, typename Future>
    inline static void setFuturesMap(std::map<Key, Future> &futuresMap)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        T::getFuturesMap() = futuresMap;
        mutex.unlock();
    }
};

}

}

#endif
