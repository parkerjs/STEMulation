#include "instanceCredentialed.h"

namespace networking
{

namespace authentication
{

/**
 * Constructor
 */
InstanceCredentialed::InstanceCredentialed(void)
{

}

/**
 * Copy constructor
 */
InstanceCredentialed::InstanceCredentialed(const InstanceCredentialed &credentialed)
{
    operator = (credentialed);
}

/**
 * Move constructor
 */
InstanceCredentialed::InstanceCredentialed(InstanceCredentialed &&credentialed)
{
    operator = (std::move(credentialed));
}

/**
 * Destructor
 */
InstanceCredentialed::~InstanceCredentialed(void)
{

}

/**
 * Copy assignment operator
 */
InstanceCredentialed &InstanceCredentialed::operator = (const InstanceCredentialed &credentialed)
{
    if (&credentialed != this)
    {
        m_credentialsMap = credentialed.m_credentialsMap;
    }

    return *this;
}

/**
 * Move assignment operator
 */
InstanceCredentialed &InstanceCredentialed::operator = (InstanceCredentialed &&credentialed)
{
    if (&credentialed != this)
    {
        m_credentialsMap = std::move(credentialed.m_credentialsMap);
    }

    return *this;
}

/**
 * Function to add a name-credential value pair
 */
std::string &InstanceCredentialed::addCredential(const std::string &name,
                                                 const std::string &value)
{
    auto &&itCredentialEntry = m_credentialsMap.emplace(name, value);

    return itCredentialEntry.first->second;
}

/**
 * Get a map of this object's name-value credential pairs
 */
std::map<std::string, std::string> &InstanceCredentialed::getCredentialsMap(void)
{
    return m_credentialsMap;
}

/**
 * Function to retrieve the associated credential value for the given name; upon success, value is populated
 * and function returns true
 */
bool InstanceCredentialed::getCredential(const std::string &name,
                                         std::string &value) const
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
bool InstanceCredentialed::setCredential(const std::string &name,
                                         const std::string &value)
{
    auto &&itCredential = m_credentialsMap.find(name);
    bool bSuccess = (itCredential != m_credentialsMap.cend());
    if (bSuccess)
        itCredential->second = value;

    return bSuccess;
}

}

}
