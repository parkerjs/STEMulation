#ifndef LOGGABLE_H
#define LOGGABLE_H

#include "logging_level.h"
#include "swappable.h"
#include <map>
#include <mutex>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a concrete interface to use and manipulate logging streams
 */
template<typename Key, typename Stream>
class Loggable
: public interfaces::Swappable<Loggable<Key, Stream>>
{
public:

    /**
     * Constructor
     * @param pLoggingStreamMap a pointer to a map of logging stream objects
     */
    Loggable(std::map<Key, Stream *> *pLoggingStreamMap = nullptr)
    : m_pLoggingStreamMap(pLoggingStreamMap)
    {

    }

    /**
     * Copy constructor
     */
    Loggable(const Loggable<Key, Stream> &loggable)
    : m_pLoggingStreamMap(nullptr)
    {
        operator = (loggable);
    }

    /**
     * Move constructor
     */
    Loggable(Loggable<Key, Stream> &&loggable)
    : m_pLoggingStreamMap(nullptr)
    {
        operator = (std::move(loggable));
    }

    /**
     * Destructor
     */
    virtual ~Loggable(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Loggable<Key, Stream> &operator = (const Loggable<Key, Stream> &loggable)
    {
        if (&loggable != this)
        {
            m_pLoggingStreamMap = loggable.m_pLoggingStreamMap;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Loggable<Key, Stream> &operator = (Loggable<Key, Stream> &&loggable)
    {
        if (&loggable != this)
        {
            m_pLoggingStreamMap = std::move(loggable.m_pLoggingStreamMap);
            loggable.m_pLoggingStreamMap = nullptr;
        }

        return *this;
    }

    /**
     * Add a logging stream; returns true upon success
     * @param key    the key associated with the logging stream object
     * @param stream a reference to a logging stream object
     */
    virtual bool addLoggingStream(const Key &key,
                                  Stream &stream) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pLoggingStreamMap != nullptr);
        if (bSuccess)
            m_pLoggingStreamMap->emplace(key, &stream);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Get a pointer to a logging stream specified by key; returns a non-null pointer to a logging stream
     * object upon success
     * @param key the key associated with the logging stream object to be retrieved
     */
    virtual Stream *getLoggingStream(const Key &key) const final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        Stream *pLoggingStream = nullptr;
        if (m_pLoggingStreamMap != nullptr)
        {
            auto &&itLoggingStreamEntry = m_pLoggingStreamMap->find(key);
            if (itLoggingStreamEntry != m_pLoggingStreamMap->cend())
                pLoggingStream = itLoggingStreamEntry->second;
        }

        mutex.unlock();

        return pLoggingStream;
    }

    /**
     * Get a pointer to this object's map of logging stream objects
     */
    inline virtual std::map<Key, Stream *> *getLoggingStreamMap(void) final
    {
        return m_pLoggingStreamMap;
    }

    /**
     * Function to log a message having the specified severity level to the logging stream associated with the
     * given key. Upon success, returns true if message was logged; false otherwise.
     * @param key     the associated logger object to which the message will be written
     * @param level   the severity level of the message
     * @param message the data to be logged
     * @param sender  the name of the message sender
     */
    virtual bool logMsg(const Key &key,
                        const utilities::LoggingLevel &level,
                        const std::string &message,
                        const std::string &sender = "") const final
    {
        bool bSuccess = (m_pLoggingStreamMap != nullptr);
        if (bSuccess)
        {
            auto &&itLoggerEntry = m_pLoggingStreamMap->find(key);
            bSuccess = (itLoggerEntry != m_pLoggingStreamMap->end());
            if (bSuccess)
            {
                auto *pStream = const_cast<Stream *>(itLoggerEntry->second);
                bSuccess = (pStream != nullptr);
                if (bSuccess)
                    bSuccess = logMsg(*pStream, level, message, sender);
            }
        }

        return bSuccess;
    }

    /**
     * Function to log a message having the specified severity level to the specified stream. Upon success,
     * returns true if message was logged; false otherwise.
     * @param stream  a reference to an std::ostream object
     * @param level   the severity level of the message
     * @param message the data to be logged
     * @param sender  the name of the message sender
     */
    static bool logMsg(std::ostream &stream,
                       const utilities::LoggingLevel &level,
                       const std::string &message,
                       const std::string &sender = "")
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            if (!sender.empty())
            {
                switch (level)
                {
                    default:
                    case utilities::LoggingLevel::Enum::Debug: stream << "Message"; break;
                    case utilities::LoggingLevel::Enum::Error: stream << "Error message"; break;
                    case utilities::LoggingLevel::Enum::Exception: stream << "Exception "; break;
                    case utilities::LoggingLevel::Enum::Usage: stream << "Usage"; break;
                    case utilities::LoggingLevel::Enum::Warning: stream << "Warning"; break;
                }

                stream << " from " << sender << ": ";
            }

            stream << message;
            bSuccess = !stream.fail();
        }

        return bSuccess;
    }

    /**
     * Remove a logging stream by key; returns true upon success
     * @param key the key associated with the logging stream object to be removed
     */
    virtual bool removeLoggingStream(const Key &key) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pLoggingStreamMap != nullptr);
        if (bSuccess)
        {
            auto &&itLoggingStreamEntry = m_pLoggingStreamMap->find(key);
            bool bSuccess = (itLoggingStreamEntry != m_pLoggingStreamMap->cend());
            if (bSuccess)
                m_pLoggingStreamMap->erase(itLoggingStreamEntry);
        }

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Remove a logging stream by value; returns true upon success
     * @param pStream a pointer to the logging stream object to be removed
     */
    virtual bool removeLoggingStream(Stream *pStream) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pLoggingStreamMap != nullptr);
        if (bSuccess)
        {
            auto &&itLoggingStreamEntry = m_pLoggingStreamMap->begin();
            while (itLoggingStreamEntry != m_pLoggingStreamMap->end())
            {
                if (itLoggingStreamEntry->second == pStream)
                    itLoggingStreamEntry = m_pLoggingStreamMap->erase(itLoggingStreamEntry);
                else
                    ++itLoggingStreamEntry;
            }
        }

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set a logging stream specified by key; returns true upon success
     * @param key    the key associated with the logging stream object
     * @param stream a reference to a logging stream object
     */
    virtual bool setLoggingStream(const Key &key,
                                  Stream &stream) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        bool bSuccess = (m_pLoggingStreamMap != nullptr);
        if (bSuccess)
            m_pLoggingStreamMap->emplace(key, &stream);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Set this object's map of logging stream objects
     * @param loggingStreamMap a reference to a map of logging stream objects
     */
    virtual void setLoggingStreamMap(std::map<Key, Stream *> &loggingStreamMap) final
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        m_pLoggingStreamMap = &loggingStreamMap;
        mutex.unlock();
    }

    /**
     * Swap function
     */
    inline virtual void swap(Loggable<Key, Stream> &loggable) override final
    {
        std::swap(m_pLoggingStreamMap, loggable.m_pLoggingStreamMap);
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
