#ifndef LOGGING_STREAMABLE_H
#define LOGGING_STREAMABLE_H

#include <map>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of logging streams - along with a concrete interface to use and
 * manipulate the streams
 */
template<typename Key, typename Stream>
class LoggingStreamable
{
public:

    /**
     * Constructor
     */
    LoggingStreamable(void)
    : m_pLoggingStreamMap(new std::map<Key, Stream *>())
    {

    }

    /**
     * Copy constructor
     */
    LoggingStreamable(const LoggingStreamable<Key, Stream> &streamable)
    : m_pLoggingStreamMap(nullptr)
    {
        operator = (streamable);
    }

    /**
     * Move constructor
     */
    LoggingStreamable(LoggingStreamable<Key, Stream> &&streamable)
    : m_pLoggingStreamMap(nullptr)
    {
        operator = (std::move(streamable));
    }

    /**
     * Destructor
     */
    virtual ~LoggingStreamable(void)
    {
        if (m_pLoggingStreamMap != nullptr)
            delete m_pLoggingStreamMap;
    }

    /**
     * Copy assignment operator
     */
    LoggingStreamable<Key, Stream> &operator = (const LoggingStreamable<Key, Stream> &streamable)
    {
        if (&streamable != this)
        {
            if (m_pLoggingStreamMap != nullptr)
            {
                delete m_pLoggingStreamMap;
                m_pLoggingStreamMap = nullptr;
            }

            if (streamable.m_pLoggingStreamMap != nullptr)
                m_pLoggingStreamMap = new std::map<Key, Stream *>(*streamable.m_pLoggingStreamMap);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    LoggingStreamable<Key, Stream> &operator = (LoggingStreamable<Key, Stream> &&streamable)
    {
        if (&streamable != this)
        {
            if (m_pLoggingStreamMap != nullptr)
                delete m_pLoggingStreamMap;

            m_pLoggingStreamMap = std::move(streamable.m_pLoggingStreamMap);
            streamable.m_pLoggingStreamMap = nullptr;
        }

        return *this;
    }

    /**
     * Get a reference to this object's map of logging stream objects
     */
    inline virtual std::map<Key, Stream *> *getLoggingStreamMap(void) final
    {
        return m_pLoggingStreamMap;
    }

protected:

    /**
     * map of logging stream objects
     */
    std::map<Key, Stream *> *m_pLoggingStreamMap;
};

}

}

#endif
