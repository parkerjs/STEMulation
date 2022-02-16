#ifndef ARITHMETIC_ATTRIBUTES_H
#define ARITHMETIC_ATTRIBUTES_H

#include "arithemtic_addable.h"
#include "arithemtic_decrementable.h"
#include "arithemtic_divisible.h"
#include "arithemtic_incrementable.h"
#include "arithemtic_modulable.h"
#include "arithemtic_multipliable.h"
#include "arithemtic_subtractable.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class inherit elementary arithmetic operator overloads (+, /, *, -)
 */
template<typename T>
class ElementaryArithmeticAttributes
: public Addable<T>,
  public Divisible<T>,
  public Multipliable<T>,
  public Subtractable<T>
{
public:

    /**
     * Destructor
     */
    virtual ~ElementaryArithmeticAttributes(void) override
    {

    }
};

/**
 * Objects derived from this class inherit arithmetic operator overloads (--, ++, *, /, %, -, +)
 */
template<typename T>
class ArithmeticAttributes
: public Decrementable<T>,
  public ElementaryArithmeticAttributes<T>,
  public Incrementable<T>,
  public Modulable<T>
{
public:

    /**
     * Destructor
     */
    virtual ~ArithmeticAttributes(void) override
    {

    }
};

}

}

}

#endif
