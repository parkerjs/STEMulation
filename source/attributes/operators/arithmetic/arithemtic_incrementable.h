#ifndef INCREMENTABLE_H
#define INCREMENTABLE_H

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class inherit the suffix increment operator overload
 */
template<typename T>
class Incrementable
{
public:

    /**
     * Suffix increment operator
     */
    friend T operator ++ (T &lhs, int)
    {
        T value(lhs);

        lhs.operator ++ ();

        return value;
    }
};

}

}

}

#endif
