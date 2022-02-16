#ifndef OUTPUT_STREAMABLE_H
#define OUTPUT_STREAMABLE_H

#include <ostream>

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that can write to output streams
 */
template<typename T, class Stream = std::ostream>
class OutputStreamable
{
public:

    /**
     * Constructor
     */
    OutputStreamable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~OutputStreamable(void)
    {

    }

    /**
     * Output stream operator
     */
    friend Stream &operator << (Stream &stream, const T &data)
    {
        return data.print(stream);
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    virtual Stream &print(Stream &stream) const
    {
        return stream;
    }
};

}

}

#endif
