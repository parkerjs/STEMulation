#ifndef TUPLE_ATTRIBUTE_H
#define TUPLE_ATTRIBUTE_H

#include <tuple>

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a tuple container consisting of the types specified within the
 * template argument list
 */
template<typename Arg, typename ... Args>
class TupleAttribute
{
public:

    /**
     * Constructor
     */
    TupleAttribute(void)
    : m_pTuple(new std::tuple<Arg, Args ...>())
    {

    }

    /**
     * Constructor
     */
    TupleAttribute(std::tuple<Arg, Args ...> *&&pTuple)
    : m_pTuple(std::move(pTuple))
    {

    }

    /**
     * Copy constructor
     */
    TupleAttribute(const TupleAttribute<Arg, Args ...> &attribute)
    : m_pTuple(nullptr)
    {
        operator = (attribute);
    }

    /**
     * Move constructor
     */
    TupleAttribute(TupleAttribute<Arg, Args ...> &&attribute)
    : m_pTuple(nullptr)
    {
        operator = (std::move(attribute));
    }

    /**
     * Destructor
     */
    virtual ~TupleAttribute(void)
    {
        if (m_pTuple != nullptr)
            delete m_pTuple;
    }

    /**
     * Copy assignment operator
     */
    TupleAttribute<Arg, Args ...> &operator = (const TupleAttribute<Arg, Args ...> &attribute)
    {
        if (&attribute != this)
        {
            if (attribute.m_pTuple != nullptr)
            {
                if (m_pTuple == nullptr)
                    m_pTuple = new std::tuple<Arg, Args ...>();

                *m_pTuple = *attribute.m_pTuple;
            }
            else
            {
                if (m_pTuple != nullptr)
                {
                    delete m_pTuple;
                    m_pTuple = nullptr;
                }
            }
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    TupleAttribute<Arg, Args ...> &operator = (TupleAttribute<Arg, Args ...> &&attribute)
    {
        if (&attribute != this)
        {
            if (m_pTuple != nullptr)
            {
                delete m_pTuple;
                m_pTuple = nullptr;
            }

            m_pTuple = std::move(attribute.m_pTuple);
            attribute.m_pTuple = nullptr;
        }

        return *this;
    }

protected:

    /**
     * pointer to an std::tuple
     */
    std::tuple<Arg, Args ...> *m_pTuple;
};

}

}

#endif
