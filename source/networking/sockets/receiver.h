#ifndef RECEIVER_H
#define RECEIVER_H

#include "cloneable.h"
#include "factory_constructible.h"
#include "reflective.h"
#include "setupable.h"
#include "static_loggable.h"
#include "static_logging_streamable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "toggleable_stream.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"

namespace networking
{

// forward declarations
class HttpHeaders;

namespace sockets
{

// forward declarations
class Socket;

/**
 * This abstract class can be used to implement data receivers using socket programming
 */
class Receiver
: public attributes::interfaces::Cloneable<Receiver>,
  public attributes::abstract::FactoryConstructible<Receiver>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::concrete::StaticLoggable<Receiver>,
  public attributes::concrete::StaticLoggingStreamable<Receiver, std::string,
                                                       utilities::ToggleableStream>,
  public attributes::concrete::StaticMutexMappable<Receiver, std::string, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Receiver>,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::concrete::VariableMapRegistrable
{
protected:

    /**
     * Typedef declarations
     */
    typedef std::function<bool (std::string &)> functor_type;

    /**
     * Constructor
     * @param pSocket           a pointer to a socket object
     * @param receiveBufferSize the size of the receive buffer
     */
    EXPORT_STEM Receiver(Socket *pSocket,
                         long receiveBufferSize = 1024);

    /**
     * Copy constructor
     */
    EXPORT_STEM Receiver(const Receiver &receiver);

    /**
     * Move constructor
     */
    EXPORT_STEM Receiver(Receiver &&receiver);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Receiver(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Receiver &operator = (const Receiver &receiver);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Receiver &operator = (Receiver &&receiver);

public:

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     * @param pHttpResponseHeaders a pointer to an HttpHeaders container, which upon success will be populated
     *                             with the HTTP response headers
     * @param pSocket              a pointer to a socket object
     * @param receiveBufferSize    the size of the receive buffer
     */
    static EXPORT_STEM Receiver *create(HttpHeaders *pHttpResponseHeaders,
                                        Socket *pSocket,
                                        long receiveBufferSize = 1024);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

protected:

    /**
     * Get message size. Default implementation returns -1 if the message size couldn't be pre-determined from
     * the header token map.
     */
    inline virtual long getMessageSize(void) const final
    {
        return m_messageSize;
    }

public:

    /**
     * Get receive buffer
     */
    inline virtual char *getReceiveBuffer(void) const final
    {
        return m_pReceiveBuffer;
    }

    /**
     * Get receive buffer size
     */
    inline virtual long getReceiveBufferSize(void) const final
    {
        return m_receiveBufferSize;
    }

    /**
     * Get a pointer to this object's Socket object
     */
    EXPORT_STEM virtual Socket *getSocket(void) final;

    /**
     * Function to receive the data
     * @param  data         the buffer in which the received data will be stored
     * @param  stopCriteria a callback function that the receiver uses to test when to terminate retrieval
     * @return              -1 on error, 0 if connection was closed properly; otherwise, returns number of
     *                      bytes read
     */
    EXPORT_STEM virtual long receive(std::string &data,
                                     const functor_type &stopCriteria) = 0;

    /**
     * Function to receive the data
     * @param data the buffer in which the received data will be stored
     */
    inline virtual long receive(std::string &data) final
    {
        return receive(data, [] (std::string &) { return false; });
    }

    /**
     * Set the size of the message to be received
     */
    inline virtual void setMessageSize(long messageSize) final
    {
        m_messageSize = messageSize;
    }

    /**
     * Set receive buffer size
     */
    EXPORT_STEM virtual void setReceiveBufferSize(long size) final;

    /**
     * Set a pointer to this object's Socket object
     */
    EXPORT_STEM virtual bool setSocket(Socket *pSocket) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

protected:

    /**
     * length of the message
     */
    long m_messageSize;

    /**
     * receive buffer
     */
    char *m_pReceiveBuffer;

    /**
     * pointer to a socket object
     */
    Socket *m_pSocket;

    /**
     * receive buffer size
     */
    long m_receiveBufferSize;
};

}

}

#endif
