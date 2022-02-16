#ifndef SYNCHRONIZABLE_H
#define SYNCHRONIZABLE_H

#include <map>
#include <mutex>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of mutex locks for thread synchronization - along with a
 * concrete interface to use and manipulate the mutexes
 */
template<typename Key, typename Mutex>
class Synchronizable
{
protected:

    /**
     * Constructor
     * @param pMutexMap a pointer to a map of mutex objects
     */
    Synchronizable(std::map<Key, Mutex *> *pMutexMap = nullptr)
    : m_pMutexMap(pMutexMap)
    {

    }

    /**
     * Copy constructor
     */
    Synchronizable(const Synchronizable<Key, Mutex> &synchronizable)
    : m_pMutexMap(nullptr)
    {
        operator = (synchronizable);
    }

    /**
     * Move constructor
     */
    Synchronizable(Synchronizable<Key, Mutex> &&synchronizable)
    : m_pMutexMap(nullptr)
    {
        operator = (std::move(synchronizable));
    }

public:

    /**
     * Destructor
     */
    virtual ~Synchronizable(void)
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    Synchronizable<Key, Mutex> &operator = (const Synchronizable<Key, Mutex> &synchronizable)
    {
        if (&synchronizable != this)
        {
            m_pMutexMap = synchronizable.m_pMutexMap;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Synchronizable<Key, Mutex> &operator = (Synchronizable<Key, Mutex> &&synchronizable)
    {
        if (&synchronizable != this)
        {
            m_pMutexMap = std::move(synchronizable.m_pMutexMap);
            synchronizable.m_pMutexMap = nullptr;
        }

        return *this;
    }

public:

    /**
     * Add a mutex to this object; returns true upon success
     * @param key    the key associated with the given mutex object
     * @param pMutex a pointer to a mutex object
     */
    inline bool addMutex(const Key &key, Mutex *pMutex)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pMutexMap != nullptr);
        if (bSuccess)
            m_pMutexMap->emplace(key, pMutex);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Get a pointer to a mutex specified by key
     * @param  key the key associated with the mutex object to be retrieved
     * @return     a non-null pointer to the associated mutex, if found
     */
    inline virtual Mutex *getMutex(const Key &key = Key{ }) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        Mutex *pMutex = nullptr;
        if (m_pMutexMap != nullptr)
        {
            auto &&itMutexEntry = m_pMutexMap->find(key);
            if (itMutexEntry != m_pMutexMap->cend())
                pMutex = itMutexEntry->second;
        }

        mutex.unlock();

        return pMutex;
    }

    /**
     * Get a pointer to this object's mutex map
     */
    inline virtual std::map<Key, Mutex *> *getMutexMap(void) final
    {
        return m_pMutexMap;
    }

    /**
     * Function to lock a mutex specified by key
     * @param key the key associated with the mutex object to be locked
     */
    inline virtual void lock(const Key &key = Key{ }) final
    {
        auto *pMutex = getMutex(key);
        if (pMutex != nullptr)
            pMutex->lock();
    }

    /**
     * Function to lock a mutex specified by key
     * @param key the key associated with the mutex object to be locked
     */
    inline virtual void lock(const Key &key = Key{ }) const final
    {
        auto *pMutex = const_cast<Synchronizable<Key, Mutex> *>(this)->getMutex(key);
        if (pMutex != nullptr)
            pMutex->lock();
    }

    /**
     * Remove a mutex
     * @param key the key associated with the mutex object to be removed
     */
    inline virtual bool removeMutex(const Key &key = Key{ }) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pMutexMap != nullptr);
        if (bSuccess)
        {
            auto &&itMutexEntry = m_pMutexMap->find(key);
            bool bSuccess = (itMutexEntry != m_pMutexMap->cend());
            if (bSuccess)
                m_pMutexMap->erase(itMutexEntry);
        }

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set a mutex specified by key; returns true upon success
     * @param key    the key associated with the mutex object
     * @param pMutex a pointer to a mutex object
     */
    inline virtual bool setMutex(const Key &key, Mutex *pMutex) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pMutexMap != nullptr);
        if (bSuccess)
            m_pMutexMap->emplace(key, pMutex);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set this object's map of mutex objects
     * @param mutexMap a reference to a map of mutex objects
     */
    inline virtual void setMutexMap(std::map<Key, Mutex *> &mutexMap) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        m_pMutexMap = &mutexMap;
        mutex.unlock();
    }

    /**
     * Try to lock a mutex specified by key; returns true upon success
     * @param key the key associated with the mutex object upon which a lock operation will be attempted
     */
    inline virtual bool tryLock(const Key &key = Key{ }) final
    {
        auto *pMutex = getMutex(key);
        bool bSuccess = (pMutex != nullptr);
        if (bSuccess)
            bSuccess = pMutex->try_lock();

        return bSuccess;
    }

    /**
     * Try to lock a mutex specified by key; returns true upon success
     * @param key the key associated with the mutex object upon which a lock operation will be attempted
     */
    inline virtual bool tryLock(const Key &key = Key{ }) const final
    {
        auto *pMutex = const_cast<Synchronizable<Key, Mutex> *>(this)->getMutex(key);
        bool bSuccess = (pMutex != nullptr);
        if (bSuccess)
            bSuccess = pMutex->try_lock();

        return bSuccess;
    }

    /**
     * Function to unlock a mutex specified by key
     * @param key the key associated with the mutex object to be unlocked
     */
    inline virtual void unlock(const Key &key = Key{ }) final
    {
        auto *pMutex = getMutex(key);
        if (pMutex != nullptr)
            pMutex->unlock();
    }

    /**
     * Function to unlock a mutex specified by key
     * @param key the key associated with the mutex object to be unlocked
     */
    inline virtual void unlock(const Key &key = Key{ }) const final
    {
        auto *pMutex = const_cast<Synchronizable<Key, Mutex> *>(this)->getMutex(key);
        if (pMutex != nullptr)
            pMutex->unlock();
    }

private:

    /**
     * pointer to map of mutex objects
     */
    mutable std::map<Key, Mutex *> *m_pMutexMap;
};

}

}

#endif
