#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <set>

namespace memory
{

/**
 * This class implements a garbage collector for dynamically-allocated objects
 */
template<typename T>
class GarbageCollector final
{
public:

    /**
     * Constructor
     */
    GarbageCollector(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~GarbageCollector(void)
    {
        deleteObjects();
    }

    /**
     * Add an object to the list of objects managed by this garbage collector
     */
    inline virtual void addObject(T *pObject) final
    {
        m_objects.insert(pObject);
    }

    /**
     * Delete an object managed by this garbage collector
     */
    virtual bool deleteObject(T *pObject) final
    {
        auto &&itObject = m_objects.find(pObject);
        bool bSuccess = (itObject != m_objects.end());
        if (bSuccess)
        {
            auto *pObjectToDelete = pObject;
            if (pObjectToDelete != nullptr)
                delete pObjectToDelete;

            // deletion may have altered the iterator
            itObject = m_objects.find(pObject);
            if (itObject != m_objects.end())
                m_objects.erase(itObject);
        }

        return bSuccess;
    }

    /**
     * Delete all objects managed by this garbage collector
     */
    virtual void deleteObjects(void) final
    {
        while (!m_objects.empty())
        {
            deleteObject(*m_objects.begin());
        }
    }

    /**
     * Get the objects managed by this garbage collector
     */
    virtual std::set<T *> &getObjects(void) final
    {
        return m_objects;
    }

    /**
     * Release an object managed by this garbage collector
     */
    virtual bool releaseObject(T *pObject) final
    {
        auto &&itObject = m_objects.find(pObject);
        bool bSuccess = (itObject != m_objects.end());
        if (bSuccess)
            m_objects.erase(itObject);

        return bSuccess;
    }

    /**
     * Release all objects managed by this garbage collector
     */
    virtual void releaseObjects(void) final
    {
        m_objects.clear();
    }

private:

    /**
     * a set of pointers to instances of type T managed by this object
     */
    std::set<T *> m_objects;
};

}

#endif
