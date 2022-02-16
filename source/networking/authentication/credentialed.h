#ifndef CREDENTIALED_H
#define CREDENTIALED_H

#include "cloneable.h"
#include <map>
#include <string>

namespace networking
{

namespace authentication
{

/**
 * This class provides an abstract interface for derived types to specify credentials for authentication
 * purposes
 */
class Credentialed
: public attributes::interfaces::Cloneable<Credentialed>
{
protected:

    /**
     * Constructor
     */
    Credentialed(void)
    {

    }

    /**
     * Copy constructor
     */
    Credentialed(const Credentialed &credentialed) = delete;

    /**
     * Move constructor
     */
    Credentialed(Credentialed &&credentialed) = delete;

public:

    /**
     * Destructor
     */
    virtual ~Credentialed(void) override
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    Credentialed &operator = (const Credentialed &credentialed) = delete;

    /**
     * Move assignment operator
     */
    Credentialed &operator = (Credentialed &&credentialed) = delete;

public:

    /**
     * Function to add a name-credential value pair
     */
    virtual std::string &addCredential(const std::string &name,
                                       const std::string &value = "") = 0;

    /**
     * clone() function
     */
    virtual Credentialed *clone(void) const override = 0;

    /**
     * Get a map of this object's name-value credential pairs
     */
    virtual std::map<std::string, std::string> &getCredentialsMap(void) = 0;

    /**
     * Function to retrieve the associated credential value for the given name; upon success, value is
     * populated and function returns true
     */
    virtual bool getCredential(const std::string &name,
                               std::string &value) const = 0;

    /**
     * Function to set the associated credential value for the given name; upon success, the value associated
     * with the given credential (given that it exists) is set and the function returns true
     */
    virtual bool setCredential(const std::string &name,
                               const std::string &value) = 0;
};

}

}

#endif
