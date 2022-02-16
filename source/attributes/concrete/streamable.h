#ifndef STREAMABLE_H
#define STREAMABLE_H

#include "input_streamable.h"
#include "output_streamable.h"

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that can read and write to input/ and output streams
 */
template<typename T, class InputStream = std::istream, class OutputStream = std::ostream>
class Streamable
: public InputStreamable<T, InputStream>,
  public OutputStreamable<T, OutputStream>
{
public:

    /**
     * Destructor
     */
    virtual ~Streamable(void) override
    {

    }
};

}

}

#endif
