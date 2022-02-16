#ifndef INPUT_STREAMABLE_H
#define INPUT_STREAMABLE_H

#include <istream>

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that can write to input streams
 */
template<typename T, class Stream = std::istream>
class InputStreamable
{
public:

    /**
     * Constructor
     */
    InputStreamable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~InputStreamable(void)
    {

    }

    /**
     * Input stream operator
     */
    friend Stream &operator >> (Stream &stream, T &data)
    {
        return data.extract(stream);
    }

    /**
     * Input stream extraction function
     * @param stream a reference to an std::istream object
     */
    virtual Stream &extract(Stream &stream)
    {
        return stream;
    }
};

}

}

#endif
