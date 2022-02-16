#include "chunkedReceiver.h"
#include "socket.h"
#include <mutex>

// file-scoped variables
static constexpr char factoryName[] = "chunked";

// using namespace declarations
using namespace attributes::abstract;
using namespace utilities;

namespace networking
{

namespace sockets
{

// register factories...
static FactoryRegistrar<Receiver> chunked_factory(factoryName, &ChunkedReceiver::create);

/**
 * Constructor
 * @param pSocket           a pointer to a socket object
 * @param receiveBufferSize the size of the receive buffer
 */
ChunkedReceiver::ChunkedReceiver(Socket *pSocket,
                                 long receiveBufferSize)
: Receiver(pSocket, receiveBufferSize)
{

}

/**
 * Copy constructor
 */
ChunkedReceiver::ChunkedReceiver(const ChunkedReceiver &receiver)
: Receiver(receiver)
{
    operator = (receiver);
}

/**
 * Move constructor
 */
ChunkedReceiver::ChunkedReceiver(ChunkedReceiver &&receiver)
: Receiver(std::move(receiver))
{
    operator = (std::move(receiver));
}

/**
 * Destructor
 */
ChunkedReceiver::~ChunkedReceiver(void)
{

}

/**
 * Copy assignment operator
 */
ChunkedReceiver &ChunkedReceiver::operator = (const ChunkedReceiver &receiver)
{
    if (&receiver != this)
    {
        Receiver::operator = (receiver);

        // TODO: add members here...
    }

    return *this;
}

/**
 * Move assignment operator
 */
ChunkedReceiver &ChunkedReceiver::operator = (ChunkedReceiver &&receiver)
{
    if (&receiver != this)
    {
        Receiver::operator = (std::move(receiver));

        // TODO: add members here...
    }

    return *this;
}

/**
 * clone() function
 */
ChunkedReceiver *ChunkedReceiver::clone(void) const
{
    auto *pReceiver = new ChunkedReceiver(*this);
    if (pReceiver != nullptr)
        pReceiver->setup();

    return pReceiver;
}

/**
 * create() function
 * @param pSocket           a pointer to a socket object
 * @param receiveBufferSize the size of the receive buffer
 */
Receiver *ChunkedReceiver::create(Socket *pSocket,
                                  long receiveBufferSize)
{
    Receiver *pReceiver = nullptr;
    if (pSocket != nullptr)
    {
        pReceiver = new ChunkedReceiver(pSocket, receiveBufferSize);
        pReceiver->setup();
    }

    return pReceiver;
}

/**
 * Read the chunk size
 */
long ChunkedReceiver::getChunkSize(void)
{
    long chunkSize = 0;
    std::stringstream stream;
    std::string data;
    if (m_pSocket != nullptr)
    {
        while (m_pSocket->readLine(&data))
        {
            if (!data.empty() && data.find_first_not_of("ABCDEFabcdef0123456789") == std::string::npos)
            {
                stream.str(data);
                stream >> std::hex >> chunkSize;
                break;
            }
        }
    }

    return chunkSize;
}

/**
 * Get the factory name of this constructible
 */
std::string ChunkedReceiver::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Function to receive the data
 * @param  data         the buffer in which the received data will be stored
 * @param  stopCriteria a callback function that the receiver uses to test when to terminate retrieval
 * @return              = -1 on error
 *                      =  0 if connection was closed properly
 *                      >  0 otherwise, number of bytes read
 */
long ChunkedReceiver::receive(std::string &data,
                              const functor_type &stopCriteria)
{
    long totalBytesRead = 0;
    if (m_pSocket != nullptr && m_pSocket->initialized())
    {
        long chunkSize = getChunkSize();
        do
        {
            long numBytesRead = 0, result = 0;
            do
            {
                long numBytesRequested = std::min(m_receiveBufferSize - 1, chunkSize - numBytesRead);
                std::memset(m_pReceiveBuffer, 0, numBytesRequested + 1);
                result = m_pSocket->read(m_pReceiveBuffer, numBytesRequested);
                if (result > 0)
                {
                    numBytesRead += result;
                    data += m_pReceiveBuffer;
                }
                else
                    return result;
            }
            while (result > 0 && numBytesRead < chunkSize);

            totalBytesRead += numBytesRead;
            if (numBytesRead != chunkSize)
            {
                lock("std_out_mutex");
                logMsg("warning", LoggingLevel::Enum::Warning,
                       "Expected chunk size was not read.\n\n",
                       getQualifiedMethodName(__func__));
                unlock("std_out_mutex");

                return -1;
            }
            else
                chunkSize = getChunkSize();
        }
        while (chunkSize > 0);

        // read the trailing CRLF...
        m_pSocket->readLine();
    }
    else
    {
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Invalid socket file descriptor. A call to setSocketFileDescriptor() must be made "
               "prior to calling receive().\n\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
    }

    return totalBytesRead;
}

}

}
