#ifndef STATICALLY_CREDENTIALED_H
#define STATICALLY_CREDENTIALED_H

#include "export_library.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <string>

namespace networking
{

namespace authentication
{

/**
 * This class provides an interface for derived types to specify authentication credentials that apply across
 * all instances of each derived type, respectively
 */
class StaticallyCredentialed
: public attributes::concrete::StaticMutexMappable<StaticallyCredentialed, int, std::mutex>,
  public attributes::concrete::StaticSynchronizable<StaticallyCredentialed>
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM StaticallyCredentialed(void);

private:

    /**
     * Copy constructor
     */
    EXPORT_STEM StaticallyCredentialed(const StaticallyCredentialed &credentialed) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM StaticallyCredentialed(StaticallyCredentialed &&credentialed) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~StaticallyCredentialed(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM StaticallyCredentialed &operator = (const StaticallyCredentialed &credentialed) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM StaticallyCredentialed &operator = (StaticallyCredentialed &&credentialed) = delete;

public:

    /**
     * Function to add a name-credential value pair
     */
    static EXPORT_STEM std::string &addCredential(const std::string &name,
                                                  const std::string &value = "");

    /**
     * Get a map of this object's name-value credential pairs
     */
    static EXPORT_STEM std::map<std::string, std::string> &getCredentialsMap(void);

    /**
     * Function to retrieve the associated credential value for the given name; upon success, value is
     * populated and function returns true
     */
    static EXPORT_STEM bool getCredential(const std::string &name,
                                          std::string &value);

    /**
     * Function to set the associated credential value for the given name; upon success, the value associated
     * with the given credential (given that it exists) is set and the function returns true
     */
    static EXPORT_STEM bool setCredential(const std::string &name,
                                          const std::string &value);

protected:

    /**
     * map of credential name-value pairs
     */
    static std::map<std::string, std::string> m_credentialsMap;
};

}

}

#endif
