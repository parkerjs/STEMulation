#ifndef DECREMENTABLE_H
#define DECREMENTABLE_H

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class inherit the suffix decrement operator overload
 */
template<typename T>
class Decrementable
{
public:

    /**
     * Suffix decrement operator
     */
    friend T operator -- (T &lhs, int)
    {
        T value(lhs);

        lhs.operator -- ();

        return value;
    }
};

}

}

}

#endif
