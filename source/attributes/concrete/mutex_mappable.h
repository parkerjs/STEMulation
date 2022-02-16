#ifndef MUTEX_MAPPABLE_H
#define MUTEX_MAPPABLE_H

#include <map>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of mutex locks for thread synchronization - along with a
 * concrete interface to use and manipulate the mutexes
 */
template<typename Key, typename Mutex>
class MutexMappable
{
public:

    /**
     * Constructor
     */
    MutexMappable(void)
    : m_pMutexMap(new std::map<Key, Mutex *>())
    {

    }

    /**
     * Copy constructor
     */
    MutexMappable(const MutexMappable<Key, Mutex> &mappable)
    : m_pMutexMap(nullptr)
    {
        operator = (mappable);
    }

    /**
     * Move constructor
     */
    MutexMappable(MutexMappable<Key, Mutex> &&mappable)
    : m_pMutexMap(nullptr)
    {
        operator = (std::move(mappable));
    }

    /**
     * Destructor
     */
    virtual ~MutexMappable(void)
    {
        if (m_pMutexMap != nullptr)
            delete m_pMutexMap;
    }

    /**
     * Copy assignment operator
     */
    MutexMappable<Key, Mutex> &operator = (const MutexMappable<Key, Mutex> &mappable)
    {
        if (&mappable != this)
        {
            if (m_pMutexMap != nullptr)
            {
                delete m_pMutexMap;
                m_pMutexMap = nullptr;
            }

            if (mappable.m_pMutexMap != nullptr)
                m_pMutexMap = new std::map<Key, Mutex *>(*mappable.m_pMutexMap);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    MutexMappable<Key, Mutex> &operator = (MutexMappable<Key, Mutex> &&mappable)
    {
        if (&mappable != this)
        {
            if (m_pMutexMap != nullptr)
                delete m_pMutexMap;

            m_pMutexMap = std::move(mappable.m_pMutexMap);
            mappable.m_pMutexMap = nullptr;
        }

        return *this;
    }

    /**
     * Get a reference to this object's map of mutex objects
     */
    inline virtual std::map<Key, Mutex *> *getMutexMap(void) final
    {
        return m_pMutexMap;
    }

protected:

    /**
     * pointer to map of mutex locks
     */
    std::map<Key, Mutex *> *m_pMutexMap;
};

}

}

#endif
