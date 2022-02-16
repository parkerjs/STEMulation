#ifndef STATIC_LOGGING_STREAMABLE_H
#define STATIC_LOGGING_STREAMABLE_H

#include <map>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a static map of logging streams, statically shared across all
 * instances of the same type - along with a concrete, static interface to use and manipulate the streams
 */
template<typename T, typename Key, typename Stream>
class StaticLoggingStreamable
{
public:

    /**
     * Constructor
     */
    StaticLoggingStreamable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticLoggingStreamable(const StaticLoggingStreamable<T, Key, Stream> &streamable) = delete;

    /**
     * Move constructor
     */
    StaticLoggingStreamable(StaticLoggingStreamable<T, Key, Stream> &&streamable) = delete;

    /**
     * Destructor
     */
    virtual ~StaticLoggingStreamable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    StaticLoggingStreamable<T, Key, Stream> &
    operator = (const StaticLoggingStreamable<T, Key, Stream> &streamable) = delete;

    /**
     * Move assignment operator
     */
    StaticLoggingStreamable<T, Key, Stream> &
    operator = (StaticLoggingStreamable<T, Key, Stream> &&streamable) = delete;

    /**
     * Get a reference to this object's logging stream map
     */
    inline static std::map<Key, Stream *> &getLoggingStreamMap(void)
    {
        static std::map<Key, Stream *> loggingStreamMap;

        return loggingStreamMap;
    }
};

}

}

#endif
