#ifndef NON_CHUNKED_RECEIVER_H
#define NON_CHUNKED_RECEIVER_H

#include "receiver.h"

namespace networking
{

namespace sockets
{

/**
 * This class can be used in conjunction with BSD sockets to receive non-chunked transfer-encoded data
 */
class NonChunkedReceiver
: public Receiver,
  virtual private attributes::abstract::Reflective
{
protected:

    /**
     * Constructor
     * @param pSocket           a pointer to a socket object
     * @param receiveBufferSize the size of the receive buffer
     */
    EXPORT_STEM NonChunkedReceiver(Socket *pSocket,
                                   long receiveBufferSize = 1024);

    /**
     * Copy constructor
     */
    EXPORT_STEM NonChunkedReceiver(const NonChunkedReceiver &receiver);

    /**
     * Move constructor
     */
    EXPORT_STEM NonChunkedReceiver(NonChunkedReceiver &&receiver);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~NonChunkedReceiver(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM NonChunkedReceiver &operator = (const NonChunkedReceiver &receiver);

    /**
     * Move assignment operator
     */
    EXPORT_STEM NonChunkedReceiver &operator = (NonChunkedReceiver &&receiver);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual NonChunkedReceiver *clone(void) const override;

    /**
     * create() function
     * @param pSocket           a pointer to a socket object
     * @param receiveBufferSize the size of the receive buffer
     */
    static EXPORT_STEM Receiver *create(Socket *pSocket,
                                        long receiveBufferSize = 1024);

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "NonChunkedReceiver";
    }

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

    /**
     * Function to receive the data
     * @param  data         the buffer in which the received data will be stored
     * @param  stopCriteria a callback function that the receiver uses to test when to terminate retrieval
     * @return              = -1 on error
     *                      =  0 if connection was closed properly
     *                      >  0 otherwise, number of bytes read
     */
    EXPORT_STEM virtual long receive(std::string &data,
                                     const functor_type &stopCriteria) override;
};

}

}

#endif
