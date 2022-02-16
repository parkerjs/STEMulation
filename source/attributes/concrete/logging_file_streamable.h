#ifndef LOGGING_FILE_STREAMABLE_H
#define LOGGING_FILE_STREAMABLE_H

#include "loggable.h"
#include "logging_streamable.h"
#include "reflective.h"
#include <fstream>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a map of file logging streams - along with a concrete interface to
 * use and manipulate the streams
 */
template<typename Key>
class LoggingFileStreamable
: virtual public Loggable<Key, std::ofstream>,
  public LoggingStreamable<Key, std::ofstream>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    LoggingFileStreamable(void)
    {
        auto *pLoggingStreamMap = LoggingStreamable<Key, std::ofstream>::getLoggingStreamMap();
        if (pLoggingStreamMap != nullptr)
            this->setLoggingStreamMap(*pLoggingStreamMap);
    }

    /**
     * Copy constructor
     */
    LoggingFileStreamable(const LoggingFileStreamable<Key> &streamable)
    {
        operator = (streamable);
    }

    /**
     * Move constructor
     */
    LoggingFileStreamable(LoggingFileStreamable<Key> &&streamable)
    {
        operator = (std::move(streamable));
    }

    /**
     * Destructor
     */
    virtual ~LoggingFileStreamable(void) override
    {
        closeLoggingFileStreams();
        deleteLoggingFileStreams();
    }

    /**
     * Copy assignment operator
     */
    LoggingFileStreamable<Key> &operator = (const LoggingFileStreamable<Key> &streamable)
    {
        if (&streamable != this)
        {
            Loggable<Key, std::ofstream>::operator = (streamable);
            LoggingStreamable<Key, std::ofstream>::operator = (streamable);

            auto *pLoggingStreamMap = LoggingStreamable<Key, std::ofstream>::getLoggingStreamMap();
            if (pLoggingStreamMap != nullptr)
                this->setLoggingStreamMap(*pLoggingStreamMap);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    LoggingFileStreamable<Key> &operator = (LoggingFileStreamable<Key> &&streamable)
    {
        if (&streamable != this)
        {
            Loggable<Key, std::ofstream>::operator = (std::move(streamable));
            LoggingStreamable<Key, std::ofstream>::operator = (std::move(streamable));

            m_loggingFileStreams.swap(streamable.m_loggingFileStreams);
        }

        return *this;
    }

    /**
     * Close a logging output file-stream by key; returns true if the logging output file-stream for the given
     * key exists and was successfully closed
     * @param key the key associated with the logging output file-stream object to be retrieved
     */
    virtual bool closeLoggingFileStream(const Key &key) final
    {
        auto *pLoggingFileStream = this->getLoggingStream(key);
        bool bSuccess = (pLoggingFileStream != nullptr);
        if (bSuccess)
        {
            bSuccess = pLoggingFileStream->is_open();
            if (bSuccess)
                pLoggingFileStream->close();
        }

        return bSuccess;
    }

    /**
     * Close a logging output file-stream by value; returns true upon success
     * @param pLoggingFileStream a pointer to the logging file-stream to be closed
     */
    virtual bool closeLoggingFileStream(std::ofstream *pLoggingFileStream)
    {
        bool bSuccess = (pLoggingFileStream != nullptr);
        if (bSuccess)
            pLoggingFileStream->close();

        return bSuccess;
    }

    /**
     * Close all logging output file-streams
     */
    virtual void closeLoggingFileStreams(void) final
    {
        auto &&itLoggingFileStream = m_loggingFileStreams.begin();
        while (itLoggingFileStream != m_loggingFileStreams.end())
        {
            closeLoggingFileStream(*itLoggingFileStream);
            ++itLoggingFileStream;
        }
    }

    /**
     * Delete a logging output file-stream by key; returns true if the logging output file-stream for the given
     * key exists and was successfully deleted; any currently open file-streams are closed
     * @param key the key associated with the logging output file-stream object to be retrieved
     */
    virtual bool deleteLoggingFileStream(const Key &key) final
    {
        closeLoggingFileStream(key);

        auto *pLoggingFileStream = this->getLoggingStream(key);
        bool bSuccess = (pLoggingFileStream != nullptr);
        if (bSuccess)
        {
            this->removeLoggingStream(pLoggingFileStream);
            delete pLoggingFileStream;
        }

        return bSuccess;
    }

    /**
     * Delete a logging output file-stream by value; returns true upon success; any currently open file-streams
     * are closed
     * @param[in,out] pLoggingFileStream a pointer to the logging file-stream to be deleted; argument is set to
     *                                   null open success
     */
    virtual bool deleteLoggingFileStream(std::ofstream *&pLoggingFileStream) final
    {
        bool bSuccess = (pLoggingFileStream != nullptr);
        if (bSuccess)
        {
            // close the file-stream
            pLoggingFileStream->close();

            auto &&itLoggingFileStreamEntry = m_loggingFileStreams.begin();
            while (itLoggingFileStreamEntry != m_loggingFileStreams.end())
            {
                if (*itLoggingFileStreamEntry == pLoggingFileStream)
                    itLoggingFileStreamEntry = m_loggingFileStreams.erase(itLoggingFileStreamEntry);
                else
                    ++itLoggingFileStreamEntry;
            }

            delete pLoggingFileStream;
            pLoggingFileStream = nullptr;
        }

        return bSuccess;
    }

    /**
     * Delete all logging output file-streams from this model
     */
    virtual void deleteLoggingFileStreams(void) final
    {
        auto &&itLoggingFileStream = m_loggingFileStreams.begin();
        while (itLoggingFileStream != m_loggingFileStreams.end())
        {
            auto *pLoggingFileStream = *itLoggingFileStream;
            if (pLoggingFileStream != nullptr)
            {
                this->removeLoggingStream(pLoggingFileStream);
                delete pLoggingFileStream;
            }

            itLoggingFileStream = m_loggingFileStreams.erase(itLoggingFileStream);
        }
    }

    /**
     * Get the name of this class
     */
    virtual std::string getClassName(void) const override
    {
        return "LoggingFileStreamable";
    }

    /**
     * Get a pointer to a logging output file-stream; returns non-null if one currently exists
     * @param key the key associated with the logging output file-stream object to be retrieved
     */
    virtual std::ofstream *getLoggingFileStream(const Key &key) const final
    {
        auto *pLoggingFileStream = this->getLoggingStream(key);
        if (pLoggingFileStream == nullptr)
        {
            this->logMsg(std::cout, utilities::LoggingLevel::Enum::Error,
                         "A file-stream associated with key \"" + key + "\" does not exist!\n",
                         getQualifiedMethodName(__func__));
        }

        return pLoggingFileStream;
    }

    /**
     * Return a pointer to a newly opened logging output file-stream; returns non-null open success. If a
     * logging output file-stream already exists for the specified key, any currently open streams will be
     * closed
     * @param key      the key to which the new logging output file-stream is to be associated
     * @param filename the name of the output file to be opened
     * @param mode     the file open mode
     */
    virtual std::ofstream *openLoggingFileStream(const Key &key,
                                                 const std::string &filename,
                                                 std::ios_base::openmode mode = std::ios_base::out)
    {
        auto *pLoggingFileStream = this->getLoggingStream(key);
        if (pLoggingFileStream == nullptr)
        {
            pLoggingFileStream = new std::ofstream(filename, mode);
            m_loggingFileStreams.push_back(pLoggingFileStream);
            this->addLoggingStream(key, *pLoggingFileStream);
        }
        else
        {
            // close the existing logging output file-stream, if already open
            if (pLoggingFileStream->is_open())
                pLoggingFileStream->close();

            pLoggingFileStream->open(filename);
            if (!pLoggingFileStream->is_open())
                deleteLoggingFileStream(key);
        }

        return pLoggingFileStream;
    }

protected:

    /**
     * vector of logging file streams
     */
    std::vector<std::ofstream *> m_loggingFileStreams;
};

}

}

#endif
