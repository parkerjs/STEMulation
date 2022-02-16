#ifndef ASYNCHRONOUS_H
#define ASYNCHRONOUS_H

#include <map>
#include <mutex>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of futures - along with a concrete interface for types that use
 * futures to obtain the results of simultaneously executing threads
 */
template<typename Key,
         typename Future>
class Asynchronous
{
protected:

    /**
     * Constructor
     * @param pFuturesMap a pointer to a map of future objects
     */
    Asynchronous(std::map<Key,
                          Future> *pFuturesMap = nullptr)
    : m_pFuturesMap(pFuturesMap)
    {

    }

    /**
     * Copy constructor
     */
    Asynchronous(const Asynchronous<Key,
                                    Future> &asynchronous)
    : m_pFuturesMap(nullptr)
    {
        operator = (asynchronous);
    }

    /**
     * Move constructor
     */
    Asynchronous(Asynchronous<Key,
                              Future> &&asynchronous)
    : m_pFuturesMap(nullptr)
    {
        operator = (std::move(asynchronous));
    }

public:

    /**
     * Destructor
     */
    virtual ~Asynchronous(void)
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    Asynchronous<Key,
                 Future> &
    operator = (const Asynchronous<Key,
                                   Future> &asynchronous)
    {
        if (&asynchronous != this)
        {
            m_pFuturesMap = asynchronous.m_pFuturesMap;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Asynchronous<Key,
                 Future> &
    operator = (Asynchronous<Key,
                             Future> &&asynchronous)
    {
        if (&asynchronous != this)
        {
            m_pFuturesMap = std::move(asynchronous.m_pFuturesMap);
            asynchronous.m_pFuturesMap = nullptr;
        }

        return *this;
    }

public:

    /**
     * Add a future; returns true upon success
     * @param key    the key associated with the given future object
     * @param future an r-value reference to a future object
     */
    inline virtual bool
    addFuture(const Key &key,
              Future &&future) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;

        mutex.lock();
        bool bSuccess = (m_pFuturesMap != nullptr);
        if (bSuccess)
        {
            m_pFuturesMap->emplace(key,
                                   std::move(future));
        }

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Get a pointer to a future specified by key
     * @param  key the key associated with the future object to be retrieved
     * @return     a non-null pointer to the associated future, if found
     */
    inline virtual Future *
    getFuture(const Key &key) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;

        mutex.lock();
        Future *pFuture = nullptr;
        if (m_pFuturesMap != nullptr)
        {
            auto &&itFutureEntry = m_pFuturesMap->find(key);
            if (itFutureEntry != m_pFuturesMap->cend())
            {
                pFuture = &itFutureEntry->second;
            }
        }

        mutex.unlock();

        return pFuture;
    }

    /**
     * Get a pointer to this object's map of futures
     */
    inline virtual std::map<Key,
                            Future> *
    getFuturesMap(void) final
    {
        return m_pFuturesMap;
    }

    /**
     * Remove a future
     * @param key the key associated with the future object to be removed
     */
    inline virtual bool
    removeFuture(const Key &key) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;

        mutex.lock();
        bool bSuccess = (m_pFuturesMap != nullptr);
        if (bSuccess)
        {
            auto &&itFutureEntry = m_pFuturesMap->find(key);
            bool bSuccess = (itFutureEntry != m_pFuturesMap->cend());
            if (bSuccess)
            {
                m_pFuturesMap->erase(itFutureEntry);
            }
        }

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set a future object specified by key; returns true upon success
     * @param key    the key associated with the given future object
     * @param future an r-value reference to a future object
     */
    inline virtual bool
    setFuture(const Key &key,
              Future &&future) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;

        mutex.lock();
        bool bSuccess = (m_pFuturesMap != nullptr);
        if (bSuccess)
        {
            m_pFuturesMap->emplace(key,
                                   std::move(future));
        }

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set this object's map of future objects
     * @param futuresMap a reference to a map of future objects
     */
    inline virtual void
    setFuturesMap(std::map<Key,
                           Future> &futuresMap) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;

        mutex.lock();
        m_pFuturesMap = &futuresMap;
        mutex.unlock();
    }

private:

    /**
     * pointer to map of future objects
     */
    std::map<Key,
             Future> *m_pFuturesMap;
};

}

}

#endif
