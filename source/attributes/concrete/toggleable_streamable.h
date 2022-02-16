#ifndef TOGGLEABLE_STREAMABLE_H
#define TOGGLEABLE_STREAMABLE_H

#include "toggleable_stream.h"
#include <map>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of streams that can be toggled on/off - along with a concrete
 * interface to use and manipulate the streams
 */
template<typename Key>
class ToggleableStreamable
{
public:

    /**
     * Constructor
     * @param pToggleableStreamMap a pointer to a map of ToggleableStream objects
     */
    ToggleableStreamable(std::map<Key, utilities::ToggleableStream *> *pToggleableStreamMap = nullptr)
    : m_pToggleableStreamMap(pToggleableStreamMap)
    {

    }

    /**
     * Copy constructor
     */
    ToggleableStreamable(const ToggleableStreamable<Key> &streamable)
    : m_pToggleableStreamMap(streamable.m_pToggleableStreamMap)
    {

    }

    /**
     * Move constructor
     */
    ToggleableStreamable(ToggleableStreamable<Key> &&streamable)
    : m_pToggleableStreamMap(std::move(streamable.m_pToggleableStreamMap))
    {
        streamable.m_pToggleableStreamMap = nullptr;
    }

    /**
     * Destructor
     */
    virtual ~ToggleableStreamable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    ToggleableStreamable<Key> &operator = (const ToggleableStreamable<Key> &streamable) = delete;

    /**
     * Move assignment operator
     */
    ToggleableStreamable<Key> &operator = (ToggleableStreamable<Key> &&streamable) = delete;

    /**
     * Add a stream; returns true upon success
     * @param key    the key associated with the ToggleableStream object
     * @param stream a reference to a ToggleableStream object
     */
    inline virtual bool addToggleableStream(const Key &key, utilities::ToggleableStream &stream) final
    {
        bool bSuccess = (m_pToggleableStreamMap != nullptr);
        if (bSuccess)
            m_pToggleableStreamMap->emplace(key, &stream);

        return bSuccess;
    }

    /**
     * Get a stream; returns a non-null pointer to a ToggleableStream object upon success
     * @param key the key associated with the ToggleableStream object to be retrieved
     */
    inline virtual utilities::ToggleableStream *getToggleableStream(const Key &key) final
    {
        utilities::ToggleableStream *pToggleableStream = nullptr;
        if (m_pToggleableStreamMap != nullptr)
        {
            auto &&itToggleableStreamEntry = m_pToggleableStreamMap->find(key);
            if (itToggleableStreamEntry != m_pToggleableStreamMap->cend())
                pToggleableStream = itToggleableStreamEntry->second;
        }

        return pToggleableStream;
    }

    /**
     * Get a reference to this object's map of ToggleableStream objects
     */
    inline virtual std::map<Key, utilities::ToggleableStream *> *getToggleableStreamMap(void) final
    {
        return m_pToggleableStreamMap;
    }

    /**
     * Remove a stream
     * @param key the key associated with the ToggleableStream object to be removed
     */
    inline virtual bool removeToggleableStream(const Key &key) final
    {
        bool bSuccess = (m_pToggleableStreamMap != nullptr);
        if (bSuccess)
        {
            auto &&itToggleableStreamEntry = m_pToggleableStreamMap->find(key);
            bool bSuccess = (itToggleableStreamEntry != m_pToggleableStreamMap->cend());
            if (bSuccess)
                m_pToggleableStreamMap->erase(itToggleableStreamEntry);
        }

        return bSuccess;
    }

    /**
     * Set this object's map of ToggleableStream objects
     * @param toggleableStreamMap a reference to a map of ToggleableStream objects
     */
    inline virtual void
    setToggleableStreamMap(std::map<Key, utilities::ToggleableStream *> &toggleableStreamMap) final
    {
        m_pToggleableStreamMap = &toggleableStreamMap;
    }

protected:

    /**
     * map of ToggleableStream objects
     */
    std::map<Key, utilities::ToggleableStream *> *m_pToggleableStreamMap;
};

}

}

#endif
