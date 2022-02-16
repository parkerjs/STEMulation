#ifndef FUTURE_MAPPABLE_H
#define FUTURE_MAPPABLE_H

#include <map>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of futures - along with a concrete interface to use and
 * manipulate the futures
 */
template<typename Key, typename Future>
class FutureMappable
{
public:

    /**
     * Constructor
     */
    FutureMappable(void)
    : m_pFutureMap(new std::map<Key, Future>())
    {

    }

    /**
     * Copy constructor
     */
    FutureMappable(const FutureMappable<Key, Future> &mappable)
    : m_pFutureMap(nullptr)
    {
        operator = (mappable);
    }

    /**
     * Move constructor
     */
    FutureMappable(FutureMappable<Key, Future> &&mappable)
    : m_pFutureMap(nullptr)
    {
        operator = (std::move(mappable));
    }

    /**
     * Destructor
     */
    virtual ~FutureMappable(void)
    {
        if (m_pFutureMap != nullptr)
            delete m_pFutureMap;
    }

    /**
     * Copy assignment operator
     */
    FutureMappable<Key, Future> &operator = (const FutureMappable<Key, Future> &mappable)
    {
        if (&mappable != this)
        {
            if (m_pFutureMap != nullptr)
            {
                delete m_pFutureMap;
                m_pFutureMap = nullptr;
            }

            if (mappable.m_pFutureMap != nullptr)
                m_pFutureMap = new std::map<Key, Future>(*mappable.m_pFutureMap);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    FutureMappable<Key, Future> &operator = (FutureMappable<Key, Future> &&mappable)
    {
        if (&mappable != this)
        {
            if (m_pFutureMap != nullptr)
                delete m_pFutureMap;

            m_pFutureMap = std::move(mappable.m_pFutureMap);
            mappable.m_pFutureMap = nullptr;
        }

        return *this;
    }

    /**
     * Get a reference to this object's map of future
     */
    inline virtual std::map<Key, Future> *getFuturesMap(void) final
    {
        return m_pFutureMap;
    }

protected:

    /**
     * pointer to map of futures
     */
    std::map<Key, Future> *m_pFutureMap;
};

}

}

#endif
