#include "staticallyCredentialed.h"

namespace networking
{

namespace authentication
{

// initialization of static member variables
std::map<std::string, std::string> StaticallyCredentialed::m_credentialsMap;

/**
 * Constructor
 */
StaticallyCredentialed::StaticallyCredentialed(void)
{

}

/**
 * Destructor
 */
StaticallyCredentialed::~StaticallyCredentialed(void)
{

}

/**
 * Function to add a name-credential value pair
 */
std::string &StaticallyCredentialed::addCredential(const std::string &name,
                                                   const std::string &value)
{
    lock(0);
    auto &&itCredentialEntry = m_credentialsMap.emplace(name, value);
    unlock(0);

    return itCredentialEntry.first->second;
}

/**
 * Get a map of this object's name-value credential pairs
 */
std::map<std::string, std::string> &StaticallyCredentialed::getCredentialsMap(void)
{
    return m_credentialsMap;
}

/**
 * Function to retrieve the associated credential value for the given name; upon success, value is populated
 * and function returns true
 */
bool StaticallyCredentialed::getCredential(const std::string &name,
                                           std::string &value)
{
    auto &&itCredential = m_credentialsMap.find(name);
    bool bSuccess = (itCredential != m_credentialsMap.cend());
    if (bSuccess)
        value = itCredential->second;

    return bSuccess;
}

/**
 * Function to set the associated credential value for the given name; upon success, the value associated with
 * the given credential (given that it exists) is set and the function returns true
 */
bool StaticallyCredentialed::setCredential(const std::string &name,
                                           const std::string &value)
{
    lock(0);
    auto &&itCredential = m_credentialsMap.find(name);
    bool bSuccess = (itCredential != m_credentialsMap.cend());
    if (bSuccess)
        itCredential->second = value;

    unlock(0);

    return bSuccess;
}

}

}
