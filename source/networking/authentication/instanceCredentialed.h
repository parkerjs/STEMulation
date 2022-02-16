#ifndef INSTANCE_CREDENTIALED_H
#define INSTANCE_CREDENTIALED_H

#include "credentialed.h"

namespace networking
{

namespace authentication
{

/**
 * This class provides methods for derived types to specify credentials for authentication purposes for a
 * single instance of the derived type
 */
class InstanceCredentialed
: public Credentialed
{
protected:

    /**
     * Constructor
     */
    EXPORT_STEM InstanceCredentialed(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM InstanceCredentialed(const InstanceCredentialed &credentialed);

    /**
     * Move constructor
     */
    EXPORT_STEM InstanceCredentialed(InstanceCredentialed &&credentialed);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~InstanceCredentialed(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM InstanceCredentialed &operator = (const InstanceCredentialed &credentialed);

    /**
     * Move assignment operator
     */
    EXPORT_STEM InstanceCredentialed &operator = (InstanceCredentialed &&credentialed);

public:

    /**
     * Function to add a name-credential value pair
     */
    EXPORT_STEM virtual std::string &addCredential(const std::string &name,
                                                   const std::string &value = "") override;

    /**
     * clone() function
     */
    inline virtual InstanceCredentialed *clone(void) const override
    {
        return new InstanceCredentialed(*this);
    }

    /**
     * Get a map of this object's name-value credential pairs
     */
    EXPORT_STEM virtual std::map<std::string, std::string> &getCredentialsMap(void) override;

    /**
     * Function to retrieve the associated credential value for the given name; upon success, value is
     * populated and function returns true
     */
    EXPORT_STEM virtual bool getCredential(const std::string &name,
                                           std::string &value) const override;

    /**
     * Function to set the associated credential value for the given name; upon success, the value associated
     * with the given credential (given that it exists) is set and the function returns true
     */
    EXPORT_STEM virtual bool setCredential(const std::string &name,
                                           const std::string &value) override;

protected:

    /**
     * map of credential name-value pairs
     */
    std::map<std::string, std::string> m_credentialsMap;
};

}

}

#endif
