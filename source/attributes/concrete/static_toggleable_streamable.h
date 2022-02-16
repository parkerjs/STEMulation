#ifndef STATIC_TOGGLEABLE_STREAMABLE_H
#define STATIC_TOGGLEABLE_STREAMABLE_H

#include "toggleable_stream.h"
#include <map>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of toggle-able streams, statically shared across all instances
 * of the same type - along with a concrete, static interface to use and manipulate the streams
 */
template<typename T>
class StaticToggleableStreamable
{
public:

    /**
     * Constructor
     */
    StaticToggleableStreamable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticToggleableStreamable(const StaticToggleableStreamable<T> &streamable) = delete;

    /**
     * Move constructor
     */
    StaticToggleableStreamable(StaticToggleableStreamable<T> &&streamable) = delete;

    /**
     * Destructor
     */
    virtual ~StaticToggleableStreamable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    StaticToggleableStreamable<T> &operator = (const StaticToggleableStreamable<T> &streamable) = delete;

    /**
     * Move assignment operator
     */
    StaticToggleableStreamable<T> &operator = (StaticToggleableStreamable<T> &&streamable) = delete;

    /**
     * Add a stream; returns true upon success
     * @param key    the key associated with the ToggleableStream object
     * @param stream a reference to a ToggleableStream object
     */
    template<typename Key>
    inline static bool addStaticToggleableStream(Key &&key, utilities::ToggleableStream &stream)
    {
        auto &toggleableStreamMap = T::getStaticLoggingMap();
        toggleableStreamMap[std::forward<Key>(key)] = &stream;

        return true;
    }

    /**
     * Retrieve a ToggleableStream object; returns a non-null pointer to a ToggleableStream object upon success
     * @param key the key associated with the ToggleableStream object to be retrieved
     */
    template<typename Key>
    inline static utilities::ToggleableStream *getStaticToggleableStream(Key &&key)
    {
        auto &toggleableStreamMap = T::getStaticLoggingMap();
        auto &&itToggleableStreamEntry = toggleableStreamMap.find(std::forward<Key>(key));
        if (itToggleableStreamEntry != toggleableStreamMap.cend())
            return itToggleableStreamEntry->second;

        return nullptr; // failed to find the desired entry
    }

    /**
     * Remove a ToggleableStream object
     * @param key the key associated with the ToggleableStream object to be removed
     */
    template<typename Key>
    inline static bool removeStaticToggleableStream(Key &&key)
    {
        auto &toggleableStreamMap = T::getStaticLoggingMap();
        auto &&itToggleableStreamEntry = toggleableStreamMap.find(std::forward<Key>(key));
        bool bSuccess = (itToggleableStreamEntry != toggleableStreamMap.cend());
        if (bSuccess)
            toggleableStreamMap.erase(itToggleableStreamEntry);

        return bSuccess;
    }
};

}

}

#endif
