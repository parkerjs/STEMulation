#ifndef XML_CONSTRUCTIBLE_H
#define XML_CONSTRUCTIBLE_H

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that can be constructed via XML node specification
 */
template<typename T, typename XML_Node>
class XML_Constructible
{
public:

    /**
     * Constructor
     */
    XML_Constructible(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    XML_Constructible(const XML_Constructible &constructible) = delete;

    /**
     * Move constructor
     */
    XML_Constructible(XML_Constructible &&constructible) = delete;

public:

    /**
     * Destructor
     */
    virtual ~XML_Constructible(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    XML_Constructible &operator = (const XML_Constructible &constructible) = delete;

    /**
     * Move assignment operator
     */
    XML_Constructible &operator = (XML_Constructible &&constructible) = delete;

public:

    /**
     * Function to construct an instance from an XML node
     */
    template<typename ... Args>
    inline static T *createFromXML(XML_Node *pNode, Args && ... args)
    {
        return T::createFromXML(pNode, std::forward<Args>(args) ...);
    }
};

}

}

#endif
