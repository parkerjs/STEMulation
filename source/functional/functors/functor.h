#ifndef FUNCTOR_H
#define FUNCTOR_H

#include "cloneable.h"
#include "output_streamable.h"

namespace functional
{

namespace functors
{

/**
 * Base class for binary/unary functors
 */
template<typename T, typename Result = T>
class Functor
: public attributes::interfaces::Cloneable<Functor<T, Result>>,
  public attributes::concrete::OutputStreamable<Functor<T, Result>>
{
public:

    /**
     * Destructor
     */
    virtual ~Functor(void) override
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual Functor<T, Result> *clone(void) const override = 0;
};

}

}

#endif
