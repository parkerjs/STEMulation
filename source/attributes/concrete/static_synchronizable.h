#ifndef STATIC_SYNCHRONIZABLE_H
#define STATIC_SYNCHRONIZABLE_H

#include "invoke.h"
#include <mutex>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a concrete, static interface to use and manipulate static mutex maps
 * for thread synchronization across instances of the same type
 */
template<typename T>
class StaticSynchronizable
{
protected:

    /**
     * Constructor
     */
    StaticSynchronizable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticSynchronizable(const StaticSynchronizable<T> &synchronizable) = delete;

    /**
     * Move constructor
     */
    StaticSynchronizable(StaticSynchronizable<T> &&synchronizable) = delete;

public:

    /**
     * Destructor
     */
    virtual ~StaticSynchronizable(void)
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    StaticSynchronizable<T> &operator = (const StaticSynchronizable<T> &synchronizable) = delete;

    /**
     * Move assignment operator
     */
    StaticSynchronizable<T> &operator = (StaticSynchronizable<T> &&synchronizable) = delete;

public:

    /**
     * Add a mutex to this object
     * @param key    the key associated with the given mutex object
     * @param pMutex a pointer to a mutex object
     */
    template<typename Key, typename Mutex>
    inline void addMutex(Key &&key, Mutex *pMutex)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &&mutexMap = T::template getMutexMap<typename std::decay<Key>::type>();
        mutexMap.emplace(std::forward<Key>(key), pMutex);
        mutex.unlock();
    }

    /**
     * Retrieve a mutex object specified by key
     * @param key the key associated with the mutex object to be retrieved
     */
    template<typename Key, typename U = typename std::decay<Key>::type,
             typename std::enable_if<std::is_convertible<U, std::string>::value &&
                                    !std::is_same<U, std::string>::value, int>::type = 0>
    inline static auto &getMutex(Key &&key)
    {
        return getMutex(std::string(std::forward<Key>(key)));
    }

    /**
     * Retrieve a mutex object specified by key
     * @param key the key associated with the mutex object to be retrieved
     */
    template<typename Key, typename U = typename std::decay<Key>::type,
             typename std::enable_if<std::is_same<U, std::string>::value ||
                                    !std::is_convertible<U, std::string>::value, int>::type = 0>
    static auto &getMutex(Key &&key)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &&mutexMap = T::template getMutexMap<U>();
        if (mutexMap.find(std::forward<Key>(key)) == mutexMap.cend())
            mutexMap.emplace(std::piecewise_construct, std::make_tuple(std::forward<Key>(key)),
                             std::make_tuple());

        mutex.unlock();

        return mutexMap[std::forward<Key>(key)];
    }

private:

    /**
     * Function to check if a mutex if valid
     */
    template<typename Mutex,
             typename std::enable_if<std::is_pointer<
             typename std::remove_reference<Mutex>::type>::value, int>::type = 0>
    inline static bool isValid(Mutex &&mutex)
    {
        return mutex != nullptr;
    }

    /**
     * Function to check if a mutex if valid
     */
    template<typename Mutex,
             typename std::enable_if<!std::is_pointer<
             typename std::remove_reference<Mutex>::type>::value, int>::type = 0>
    inline static bool isValid(Mutex &&mutex)
    {
        return true;
    }

public:

    /**
     * Function to lock a mutex specified by key
     * @param key the key associated with the mutex object to be locked
     */
    template<typename Key>
    inline static void lock(Key &&key)
    {
        auto &&mutex = getMutex(std::forward<Key>(key));
        if (isValid(mutex))
        {
            typedef typename std::remove_pointer<
                    typename std::decay<decltype(mutex)>::type>::type Mutex;

            functional::invoke(&Mutex::lock, mutex);
        }
    }

    /**
     * Remove a mutex object; returns true upon success
     * @param key the key associated with the mutex object to be removed
     */
    template<typename Key>
    inline static bool removeMutex(Key &&key)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &mutexMap = T::template getMutexMap<typename std::decay<Key>::type>();
        auto &&itMutexEntry = mutexMap.find(std::forward<Key>(key));
        bool bSuccess = (itMutexEntry != mutexMap.cend());
        if (bSuccess)
            mutexMap.erase(itMutexEntry);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set a mutex specified by key; returns true upon success
     * @param key    the key associated with the mutex object
     * @param pMutex a pointer to a mutex object
     */
    template<typename Key, typename Mutex>
    inline static void setMutex(Key &&key, Mutex *pMutex)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &mutexMap = T::template getMutexMap<typename std::decay<Key>::type>();
        mutexMap[std::forward<Key>(key)] = pMutex;
        mutex.unlock();
    }

    /**
     * Set this object's map of mutex objects
     * @param mutexMap a reference to a map of mutex objects
     */
    template<typename Key, typename Mutex>
    inline static void setMutexMap(std::map<Key, Mutex> &mutexMap)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        T::template getMutexMap<typename std::decay<Key>::type>() = mutexMap;
        mutex.unlock();
    }

    /**
     * Try to lock a mutex specified by key; returns true upon success
     * @param key the key associated with the mutex object upon which a lock operation will be attempted
     */
    template<typename Key>
    inline static bool tryLock(Key &&key)
    {
        auto &&mutex = getMutex(std::forward<Key>(key));
        if (isValid(mutex))
        {
            typedef typename std::remove_pointer<
                    typename std::decay<decltype(mutex)>::type>::type Mutex;

            return functional::invoke(&Mutex::try_lock, mutex);
        }
    }

    /**
     * Function to unlock a mutex specified by key
     * @param key the key associated with the mutex object to be unlocked
     */
    template<typename Key>
    inline static void unlock(Key &&key)
    {
        auto &&mutex = getMutex(std::forward<Key>(key));
        if (isValid(mutex))
        {
            typedef typename std::remove_pointer<
                    typename std::decay<decltype(mutex)>::type>::type Mutex;

            functional::invoke(&Mutex::unlock, mutex);
        }
    }
};

}

}

#endif
