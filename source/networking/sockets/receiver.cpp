#include "dictionary.h"
#include "httpHeaders.h"
#include "receiver.h"
#include "socket.h"
#include "static_message_dispatcher.h"

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <winsock.h>
#elif defined POSIX
#include <sys/socket.h>
#endif

// file-scoped variables
static constexpr char className[] = "Receiver";

// using namespace declarations
using namespace attributes::concrete;
using namespace messaging;
using namespace utilities;
using namespace utilities::string;

namespace networking
{

namespace sockets
{

// register static subscribers...
static StaticMessageRecipientRegistrar staticMessageRegistrar(
{ std::make_pair("StaticLoggable::setLoggingStreamMap<std::string, ToggleableStream>",
                 &Receiver::setLoggingStreamMap<std::string, ToggleableStream>),
  std::make_pair("StaticSynchronizable::setMutexMap<std::string, std::mutex *>",
                 &Receiver::setMutexMap<std::string, std::mutex *>)});

/**
 * Constructor
 * @param pSocket           a pointer to a socket object
 * @param receiveBufferSize the size of the receive buffer
 */
Receiver::Receiver(Socket *pSocket,
                   long receiveBufferSize)
: m_messageSize(0),
  m_pReceiveBuffer(nullptr),
  m_pSocket(pSocket),
  m_receiveBufferSize(0)
{
    setReceiveBufferSize(receiveBufferSize);
}

/**
 * Copy constructor
 */
Receiver::Receiver(const Receiver &receiver)
: m_pReceiveBuffer(nullptr),
  m_pSocket(nullptr),
  m_receiveBufferSize(0)
{
    operator = (receiver);
}

/**
 * Move constructor
 */
Receiver::Receiver(Receiver &&receiver)
: m_pReceiveBuffer(nullptr),
  m_pSocket(nullptr),
  m_receiveBufferSize(0)
{
    operator = (std::move(receiver));
}

/**
 * Destructor
 */
Receiver::~Receiver(void)
{
    if (m_pReceiveBuffer != nullptr)
        delete [] m_pReceiveBuffer;
}

/**
 * Copy assignment operator
 */
Receiver &Receiver::operator = (const Receiver &receiver)
{
    if (&receiver != this)
    {
        m_messageSize = receiver.m_messageSize;
        m_pSocket = receiver.m_pSocket; // do not clone this

        setReceiveBufferSize(receiver.m_receiveBufferSize);
    }

    return *this;
}

/**
 * Move assignment operator
 */
Receiver &Receiver::operator = (Receiver &&receiver)
{
    if (&receiver != this)
    {
        m_messageSize = std::move(receiver.m_messageSize);

        if (m_pReceiveBuffer != nullptr)
        {
            delete m_pReceiveBuffer;
            m_pReceiveBuffer = nullptr;
        }

        m_pReceiveBuffer = std::move(receiver.m_pReceiveBuffer);
        receiver.m_pReceiveBuffer = nullptr;

        m_pSocket = std::move(receiver.m_pSocket);
        receiver.m_pSocket = nullptr;

        m_receiveBufferSize = std::move(receiver.m_receiveBufferSize);
    }

    return *this;
}

/**
 * Configure function (using a variable token map)
 */
bool Receiver::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}

/**
 * create() function
 * @param pHttpResponseHeaders a pointer to an HttpHeaders container, which upon success will be populated with
 *                             the HTTP response headers
 * @param pSocket              a pointer to a socket object
 * @param receiveBufferSize    the size of the receive buffer
 */
Receiver *Receiver::create(HttpHeaders *pHttpResponseHeaders,
                           Socket *pSocket,
                           long receiveBufferSize)
{
    Receiver *pReceiver = nullptr;
    if (pHttpResponseHeaders != nullptr && pSocket != nullptr && pHttpResponseHeaders->readHeaders(pSocket))
    {
        auto &&transferEncoding = pHttpResponseHeaders->getTransferEncoding();
        if (transferEncoding.empty())
            transferEncoding = "non-chunked";

        pReceiver = FactoryConstructible<Receiver>::create(transferEncoding, pSocket, receiveBufferSize);
        if (pReceiver != nullptr)
        {
            pReceiver->setup();

            auto &&messageSize = pHttpResponseHeaders->getContentLength();
            pReceiver->setMessageSize(messageSize);
        }
    }

    return pReceiver;
}

/**
 * Get the name of this class
 */
std::string Receiver::getClassName(void) const
{
    return className;
}

/**
 * Get a pointer to this object's Socket object
 */
Socket *Receiver::getSocket(void)
{
    return m_pSocket;
}

/**
 * Set receive buffer size
 */
void Receiver::setReceiveBufferSize(long size)
{
    if (size != m_receiveBufferSize || m_pReceiveBuffer == nullptr)
    {
        if (m_pReceiveBuffer != nullptr)
            delete [] m_pReceiveBuffer;

        m_pReceiveBuffer = new char[size];
    }

    m_receiveBufferSize = size;
}

/**
 * Set a pointer to this object's Socket object
 */
bool Receiver::setSocket(Socket *pSocket)
{
    bool bSuccess = (pSocket != nullptr);
    if (bSuccess)
        m_pSocket = pSocket;

    return bSuccess;
}

/**
 * Setup function
 */
bool Receiver::setup(void)
{
    // setup token map-configurable variables...
    m_registry["receiveBufferSize"] = m_receiveBufferSize;

    return true;
}

}

}
