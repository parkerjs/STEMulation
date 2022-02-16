#ifndef STATIC_LOGGABLE_H
#define STATIC_LOGGABLE_H

#include "logging_level.h"
#include <map>
#include <mutex>
#include <string>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a concrete, static interface to statically use and manipulate the
 * logging streams
 */
template<typename T>
class StaticLoggable
{
public:

    /**
     * Constructor
     */
    StaticLoggable(void)
    {

    }

    /**
     * Copy constructor
     */
    StaticLoggable(const StaticLoggable<T> &loggable) = delete;

    /**
     * Move constructor
     */
    StaticLoggable(StaticLoggable<T> &&loggable) = delete;

    /**
     * Destructor
     */
    virtual ~StaticLoggable(void)
    {

    }

    /**
     * Copy assignment operator
     */
    StaticLoggable<T> &operator = (const StaticLoggable<T> &loggable) = delete;

    /**
     * Move assignment operator
     */
    StaticLoggable<T> &operator = (StaticLoggable<T> &&loggable) = delete;

    /**
     * Add a logging stream to this object
     * @param key    the key associated with the given logging stream object
     * @param stream a reference to a logging stream object
     */
    template<typename Key, typename Stream>
    inline static bool addLoggingStream(Key &&key, Stream &&stream)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &loggingStreamMap = T::getLoggingStreamMap();
        auto &&itLoggingStreamEntry = loggingStreamMap.find(std::forward<Key>(key));
        bool bSuccess = (itLoggingStreamEntry == loggingStreamMap.end());
        if (bSuccess)
            loggingStreamMap.emplace(key, &std::forward<Stream>(stream));

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Retrieve a logging stream object; returns a non-null pointer to a logging stream object upon success
     * @param key the key associated with the logging stream object to be retrieved
     */
    template<typename Key>
    inline static auto *getLoggingStream(Key &&key)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &loggingStreamMap = T::getLoggingStreamMap();
        auto &&itLoggingStreamEntry = loggingStreamMap.find(std::forward<Key>(key));

        typedef typename std::remove_pointer<
                typename std::decay<decltype(itLoggingStreamEntry->second)>::type>::type Stream;

        Stream *pStream = nullptr;
        if (itLoggingStreamEntry != loggingStreamMap.cend())
            pStream = itLoggingStreamEntry->second;

        mutex.unlock();

        return pStream; // failed to find the desired entry
    }

    /**
     * Function to log a message having the specified severity level to the logging stream associated with the
     * given key. Upon success, returns true if message was logged; false otherwise.
     * @param key     the key associated with the logger object to which the message will be written
     * @param level   the severity level of the message
     * @param message the data to be logged
     * @param sender  the name of the message sender
     */
    template<typename Key, typename U = typename std::remove_pointer<typename std::decay<Key>::type>::type,
             typename V = typename std::add_lvalue_reference<U>::type,
             typename std::enable_if<!std::is_convertible<V, std::ostream &>::value, int>::type = 0>
    static bool logMsg(Key &&key, const utilities::LoggingLevel &level, const std::string &message,
                       const std::string &sender = "")
    {
        auto *pStream = getLoggingStream(std::forward<Key>(key));
        bool bSuccess = (pStream != nullptr);
        if (bSuccess)
            bSuccess = logMsg(*pStream, level, message, sender);

        return bSuccess;
    }

    /**
     * Function to log a message having the specified severity level to the specified stream. Upon success,
     * returns true if message was logged; false otherwise.
     * @param stream  a reference to a stream-like object
     * @param level   the severity level of the message
     * @param message the data to be logged
     * @param sender  the name of the message sender
     */
    template<typename U, typename V = typename std::decay<U>::type,
             typename std::enable_if<!std::is_pointer<V>::value &&
                                      std::is_convertible<U, std::ostream &>::value, int>::type = 0>
    inline static bool logMsg(U &&stream, const utilities::LoggingLevel &level, const std::string &message,
                              const std::string &sender = "")
    {
        return logMsg(&std::forward<U>(stream), level, message, sender);
    }

    /**
     * Function to log a message having the specified severity level to the specified stream. Upon success,
     * returns true if message was logged; false otherwise.
     * @param pStream a pointer to a stream-like object
     * @param level   the severity level of the message
     * @param message the data to be logged
     * @param sender  the name of the message sender
     */
    template<typename U, typename V = typename std::decay<U>::type,
             typename W = typename std::add_lvalue_reference<typename std::remove_pointer<V>::type>::type,
             typename std::enable_if<std::is_pointer<V>::value &&
                                     std::is_convertible<W, std::ostream &>::value, int>::type = 0>
    static bool logMsg(U &&pStream, const utilities::LoggingLevel &level, const std::string &message,
                       const std::string &sender = "")
    {
        bool bSuccess = (pStream != nullptr);
        if (bSuccess)
        {
            switch (level)
            {
                default:
                case utilities::LoggingLevel::Enum::Debug: *pStream << "Message"; break;
                case utilities::LoggingLevel::Enum::Error: *pStream << "Error message"; break;
                case utilities::LoggingLevel::Enum::Exception: *pStream << "Exception "; break;
                case utilities::LoggingLevel::Enum::Usage: *pStream << "Usage"; break;
                case utilities::LoggingLevel::Enum::Warning: *pStream << "Warning"; break;
            }

            if (!sender.empty())
                *pStream << " from " << sender << ": ";

            *pStream << message;
        }

        return bSuccess;
    }

    /**
     * Remove a logging stream by key; returns true upon success
     * @param key the key associated with the logging stream object to be removed
     */
    template<typename Key>
    inline static bool removeLoggingStream(Key &&key)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &loggingStreamMap = T::getLoggingStreamMap();
        auto &&itLoggingStreamEntry = loggingStreamMap.find(std::forward<Key>(key));
        bool bSuccess = (itLoggingStreamEntry != loggingStreamMap.cend());
        if (bSuccess)
            loggingStreamMap.erase(itLoggingStreamEntry);

        mutex.unlock();

        return bSuccess;
    }

    /**
     * Remove a logging stream by value
     * @param pStream a pointer to the logging stream object to be removed
     */
    template<typename Stream>
    inline static void removeLoggingStream(Stream *pStream)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &loggingStreamMap = T::getLoggingStreamMap();
        auto &&itLoggingStreamEntry = loggingStreamMap.begin();
        while (itLoggingStreamEntry != loggingStreamMap.end())
        {
            if (itLoggingStreamEntry->second == pStream)
                itLoggingStreamEntry = loggingStreamMap.erase(itLoggingStreamEntry);
            else
                ++itLoggingStreamEntry;
        }

        mutex.unlock();
    }

    /**
     * Set a logging stream; returns true upon success
     * @param key    the key associated with the logging stream object
     * @param stream a reference to a logging stream object
     */
    template<typename Key, typename Stream>
    inline static void setLoggingStream(Key &&key, Stream &stream)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        auto &loggingStreamMap = T::getLoggingStreamMap();
        loggingStreamMap[std::forward<Key>(key)] = &stream;
        mutex.unlock();
    }

    /**
     * Set this object's map of logging stream objects
     * @param loggingStreamMap a reference to a map of logging stream objects
     */
    template<typename Key, typename Stream>
    inline static void setLoggingStreamMap(std::map<Key, Stream *> &loggingStreamMap)
    {
        // a mutex is needed to access the map
        static std::mutex mutex;
        mutex.lock();
        T::getLoggingStreamMap() = loggingStreamMap;
        mutex.unlock();
    }
};

}

}

#endif
