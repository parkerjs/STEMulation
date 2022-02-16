#include "stateMap.h"

namespace math
{

namespace control_systems
{

/**
 * Constructor
 * @param time             the time associated with this object
 * @param availabilityTime the availability time associated with this object
 */
StateMap::StateMap(double time,
                   double availabilityTime)
: m_availabilityTime(availabilityTime),
  m_time(time)
{

}

/**
 * Constructor
 * @param name             the name associated with this object
 * @param time             the time associated with this object
 * @param availabilityTime the availability time associated with this object
 */
StateMap::StateMap(const std::string &name,
                   double time,
                   double availabilityTime)
: m_availabilityTime(availabilityTime),
  m_name(name),
  m_time(time)
{

}

/**
 * Constructor
 * @param stateMap         a map of states used to initialize this object
 * @param time             the time associated with this object
 * @param availabilityTime the availability time associated with this object
 */
StateMap::StateMap(const std::map<std::string, double> &stateMap,
                   double time,
                   double availabilityTime)
: m_availabilityTime(availabilityTime),
  m_stateMap(stateMap),
  m_time(time)
{

}

/**
 * Constructor
 * @param stateMap         a map of states used to initialize this object
 * @param name             the name associated with this object
 * @param time             the time associated with this object
 * @param availabilityTime the availability time associated with this object
 */
StateMap::StateMap(const std::map<std::string, double> &stateMap,
                   const std::string &name,
                   double time,
                   double availabilityTime)
: m_availabilityTime(availabilityTime),
  m_name(name),
  m_stateMap(stateMap),
  m_time(time)
{

}

/**
 * Copy constructor
 */
StateMap::StateMap(const StateMap &stateMap)
{
    operator = (stateMap);
}

/**
 * Move constructor
 */
StateMap::StateMap(StateMap &&stateMap)
{
    operator = (std::move(stateMap));
}

/**
 * Destructor
 */
StateMap::~StateMap(void)
{

}

/**
 * Copy assignment operator
 */
StateMap &StateMap::operator = (const StateMap &stateMap)
{
    if (&stateMap != this)
    {
        m_availabilityTime = stateMap.m_availabilityTime;
        m_name = stateMap.m_name;
        m_stateMap = stateMap.m_stateMap;
        m_time = stateMap.m_time;
    }

    return *this;
}

/**
 * Move assignment operator
 */
StateMap &StateMap::operator = (StateMap &&stateMap)
{
    if (&stateMap != this)
    {
        stateMap.swap(*this);
    }

    return *this;
}

/**
 * Conversion to std::vector
 */
StateMap::operator std::vector<double>(void) const
{
    std::vector<double> state;
    auto &&itStateMap = m_stateMap.cbegin();
    while (itStateMap != m_stateMap.cend())
    {
        state.push_back(itStateMap->second);
        ++itStateMap;
    }

    return state;
}

/**
 * Subscript operator
 */
double &StateMap::operator [] (const std::string &name)
{
    return m_stateMap[name];
}

/**
 * begin() overload
 */
StateMap::iterator StateMap::begin(void)
{
    return m_stateMap.begin();
}

/**
 * cbegin() overload
 */
StateMap::const_iterator StateMap::cbegin(void) const
{
    return m_stateMap.cbegin();
}

/**
 * cend() overload
 */
StateMap::const_iterator StateMap::cend(void) const
{
    return m_stateMap.cend();
}

/**
 * Clear this object of its entries
 */
void StateMap::clear(void)
{
    m_stateMap.clear();
}

/**
 * clone() function
 */
StateMap *StateMap::clone(void) const
{
    return new StateMap(*this);
}

/**
 * Query whether or not this object contains a state associated with the given name
 */
bool StateMap::contains(const std::string &name) const
{
    return m_stateMap.find(name) != m_stateMap.cend();
}

/**
 * crbegin() overload
 */
StateMap::const_reverse_iterator StateMap::crbegin(void) const
{
    return m_stateMap.crbegin();
}

/**
 * crend() overload
 */
StateMap::const_reverse_iterator StateMap::crend(void) const
{
    return m_stateMap.crend();
}

/**
 * Function to deserialize this object's data
 */
std::istream &StateMap::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_availabilityTime, sizeof(double));
        stream.read((char *)&m_time, sizeof(double));

        std::size_t size = 0;
        stream.read((char *)&size, sizeof(std::size_t));

        double value;
        std::string name;
        for (std::size_t i = 0; stream && i < size; ++i)
        {
            std::getline(stream, name, '\0');
            stream.read((char *)&value, sizeof(double));
            m_stateMap[name] = value;
            name.clear();
        }
    }

    return stream;
}

/**
 * Test for empty variable registry
 */
bool StateMap::empty(void) const
{
    return m_stateMap.empty();
}

/**
 * end() overload
 */
StateMap::iterator StateMap::end(void)
{
    return m_stateMap.end();
}

/**
 * Erase within this object the entry associated with the input iterator
 */
StateMap::iterator StateMap::erase(iterator itStateMap)
{
    return m_stateMap.erase(itStateMap);
}

/**
 * Retrieve the state associated with the given name; returns true upon success
 */
bool StateMap::get(const std::string &name,
                   double &state) const
{
    auto &&itStateMap = m_stateMap.find(name);
    bool bSuccess = (itStateMap != m_stateMap.cend());
    if (bSuccess)
    {
        state = itStateMap->second;
    }

    return bSuccess;
}

/**
 * Get the availability time associated with this object
 */
double StateMap::getAvailabilityTime(void) const
{
    return m_availabilityTime;
}

/**
 * Get the name of this class
 */
std::string StateMap::getClassName(void) const
{
    return "StateMap";
}

/**
 * Get the name associated with this object
 */
std::string StateMap::getName(void) const
{
    return m_name;
}

/**
 * Get the time associated with this object
 */
double StateMap::getTime(void) const
{
    return m_time;
}

/**
 * Initialization function
 */
bool StateMap::initialize(void)
{
    m_availabilityTime = 0.0;
    m_stateMap.clear();
    m_time = 0.0;

    return true;
}

/**
 * Output stream print function
 * @param stream a reference to an std::ostream object
 */
std::ostream &StateMap::print(std::ostream &stream) const
{
    if (stream)
    {
        stream << "Availability time: " << m_availabilityTime << std::endl;
        stream << "Time: " << m_time << std::endl;
        auto &&itStateMap = m_stateMap.cbegin();
        while (itStateMap != m_stateMap.cend())
        {
            stream << itStateMap->first << ": "
                   << itStateMap->second << std::endl;

            ++itStateMap;
        }
    }

    return stream;
}

/**
 * rbegin() overload
 */
StateMap::reverse_iterator StateMap::rbegin(void)
{
    return m_stateMap.rbegin();
}

/**
 * rend() overload
 */
StateMap::reverse_iterator StateMap::rend(void)
{
    return m_stateMap.rend();
}

/**
 * Function to serialize this object's data
 */
std::ostream &StateMap::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_availabilityTime, sizeof(double));
        stream.write((const char *)&m_time, sizeof(double));

        auto size = m_stateMap.size();
        stream.write((const char *)&size, sizeof(std::size_t));
        auto &&itStateMap = m_stateMap.cbegin();
        while (itStateMap != m_stateMap.cend())
        {
            stream << itStateMap->first << '\0';
            stream.write((const char *)&itStateMap->second, sizeof(double));

            ++itStateMap;
        }
    }

    return stream;
}

/**
 * Set the state data associated with the specified name; if an entry doesn't exist, one is created
 */
bool StateMap::set(const std::string &name,
                   double state)
{
    m_stateMap[name] = state;

    return true;
}

/**
 * Set the availability time associated with this object
 */
void StateMap::setAvailabilityTime(double availabilityTime)
{
    m_availabilityTime = availabilityTime;
}

/**
 * Set the name associated with this object
 */
void StateMap::setName(const std::string &name)
{
    m_name = name;
}

/**
 * Set the time associated with this object
 */
void StateMap::setTime(double time)
{
    m_time = time;
}

/**
 * Return the number of entires in this object
 */
std::size_t StateMap::size(void) const
{
    return m_stateMap.size();
}

/**
 * Swap function
 */
void StateMap::swap(StateMap &stateMap)
{
    std::swap(m_availabilityTime, stateMap.m_availabilityTime);
    m_name.swap(stateMap.m_name);
    m_stateMap.swap(stateMap.m_stateMap);
    std::swap(m_time, stateMap.m_time);
}

/**
 * Function to convert a vector of state map objects into a map of state vectors keyed by name
 * @param  stateMapVector a vector of state map objects
 * @return                a map populated with vectors of states keyed by name
 */
std::map<std::string, std::vector<double>>
StateMap::toMap(const std::vector<StateMap *> &stateMapVector)
{
    std::map<std::string, std::vector<double>> stateVectorMap;
    std::vector<double> time;

    StateMap::toMap(stateMapVector,
                    stateVectorMap,
                    time);

    return stateVectorMap;
}

/**
 * Function to convert a vector of state map objects into a map of state vectors keyed by name
 * @param      stateMapVector a vector of state map objects
 * @param[out] time           a vector of times corresponding to the state map objects
 * @return                    a map populated with vectors of states keyed by name
 */
std::map<std::string, std::vector<double>>
StateMap::toMap(const std::vector<StateMap *> &stateMapVector,
                std::vector<double> &time)
{
    std::map<std::string, std::vector<double>> stateVectorMap;

    StateMap::toMap(stateMapVector,
                    stateVectorMap,
                    time);

    return stateVectorMap;
}

/**
 * Function to convert a vector of state map objects into a map of state vectors keyed by name
 * @param      stateMapVector a vector of state map objects
 * @param[out] stateVectorMap populated with vectors of states
 */
void StateMap::toMap(const std::vector<StateMap *> &stateMapVector,
                     std::map<std::string, std::vector<double>> &stateVectorMap)
{
    std::vector<double> time;

    toMap(stateMapVector,
          stateVectorMap,
          time);
}

/**
 * Function to convert a vector of state map objects into a map of state vectors keyed by name
 * @param      stateMapVector a vector of state map objects
 * @param[out] stateVectorMap populated with vectors of states
 * @param[out] time           populated with a vector of times corresponding to the state map objects
 */
void StateMap::toMap(const std::vector<StateMap *> &stateMapVector,
                     std::map<std::string, std::vector<double>> &stateVectorMap,
                     std::vector<double> &time)
{
    stateVectorMap.clear();
    time.clear();
    auto &&itStateMapVector = stateMapVector.cbegin();
    while (itStateMapVector != stateMapVector.cend())
    {
        auto *pStateMap = *itStateMapVector;
        if (pStateMap != nullptr)
        {
            time.push_back(pStateMap->getTime());
            auto &&itStateMap = pStateMap->cbegin();
            while (itStateMap != pStateMap->cend())
            {
                auto &&nameStatePair = *itStateMap;
                auto &&name = nameStatePair.first;
                auto &&state = nameStatePair.second;

                stateVectorMap[name].push_back(state);
                ++itStateMap;
            }
        }

        ++itStateMapVector;
    }
}

}

}
