#ifndef STATIC_AUTHENTICATABLE_H
#define STATIC_AUTHENTICATABLE_H

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete, static interface for types that require arbitrary forms of authentication
 */
template<typename T, typename Authentication>
class StaticAuthenticatable
{
public:

    /**
     * Constructor
     */
    StaticAuthenticatable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticAuthenticatable(void)
    {

    }

    /**
     * Retrieve this object's authentication credential(s)
     */
    inline static Authentication getStaticAuthentication(void)
    {
        return T::getStaticAuthentication();
    }

    /**
     * Set this object's authentication credential(s)
     */
    inline static void setStaticAuthentication(Authentication &&authentication)
    {
        T::setStaticAuthentication(std::forward<Authentication>(authentication));
    }
};

}

}

#endif
