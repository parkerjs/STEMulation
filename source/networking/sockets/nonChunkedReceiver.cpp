#include "nonChunkedReceiver.h"
#include "socket.h"
#include <mutex>

// file-scoped variables
static constexpr char factoryName[] = "non-chunked";

// using namespace declarations
using namespace attributes::abstract;
using namespace utilities;

namespace networking
{

namespace sockets
{

// register factories...
static FactoryRegistrar<Receiver> non_chunked_factory(factoryName, &NonChunkedReceiver::create);

/**
 * Constructor
 * @param pSocket           a pointer to a socket object
 * @param receiveBufferSize the size of the receive buffer
 */
NonChunkedReceiver::NonChunkedReceiver(Socket *pSocket, long receiveBufferSize)
: Receiver(pSocket, receiveBufferSize)
{

}

/**
 * Copy constructor
 */
NonChunkedReceiver::NonChunkedReceiver(const NonChunkedReceiver &receiver)
: Receiver(receiver)
{
    operator = (receiver);
}

/**
 * Move constructor
 */
NonChunkedReceiver::NonChunkedReceiver(NonChunkedReceiver &&receiver)
: Receiver(std::move(receiver))
{
    operator = (std::move(receiver));
}

/**
 * Destructor
 */
NonChunkedReceiver::~NonChunkedReceiver(void)
{

}

/**
 * Copy assignment operator
 */
NonChunkedReceiver &NonChunkedReceiver::operator = (const NonChunkedReceiver &receiver)
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
NonChunkedReceiver &NonChunkedReceiver::operator = (NonChunkedReceiver &&receiver)
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
NonChunkedReceiver *NonChunkedReceiver::clone(void) const
{
    auto *pReceiver = new NonChunkedReceiver(*this);
    if (pReceiver != nullptr)
        pReceiver->setup();

    return pReceiver;
}

/**
 * create() function
 * @param pSocket           a pointer to a socket object
 * @param receiveBufferSize the size of the receive buffer
 */
Receiver *NonChunkedReceiver::create(Socket *pSocket, long receiveBufferSize)
{
    Receiver *pReceiver = nullptr;
    if (pSocket != nullptr)
    {
        pReceiver = new NonChunkedReceiver(pSocket, receiveBufferSize);
        pReceiver->setup();
    }

    return pReceiver;
}

/**
 * Get the factory name of this constructible
 */
std::string NonChunkedReceiver::getFactoryName(void) const
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
long NonChunkedReceiver::receive(std::string &data, const functor_type &stopCriteria)
{
    long totalBytesRead = 0;
    if (m_pSocket != nullptr && m_pSocket->initialized())
    {
        long messageSize = getMessageSize();
        if (messageSize > 0)
        {
            while (true)
            {
                long numBytesRequested = m_receiveBufferSize - 1;
                if (messageSize >= 0)
                {
                    numBytesRequested = std::min(numBytesRequested, messageSize - totalBytesRead);
                    if (totalBytesRead >= messageSize)
                        break;
                }

                std::memset(m_pReceiveBuffer, 0, numBytesRequested + 1);
                long result = m_pSocket->read(m_pReceiveBuffer, numBytesRequested);
                if (result > 0)
                {
                    data += m_pReceiveBuffer;
                    totalBytesRead += result;
                }
                else
                    return result;

                if (stopCriteria(data))
                    break;
            }
        }
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
