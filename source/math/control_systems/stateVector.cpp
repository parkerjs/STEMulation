#include "matrix2d.h"
#include "stateVector.h"

namespace math
{

namespace control_systems
{

/**
 * Constructor
 * @param size the size of the state vector
 * @param time the time associated with this state
 */
StateVector::StateVector(std::size_t size,
                         double time)
: m_state(size, 0.0),
  m_time(time)
{

}

/**
 * Constructor
 * @param state initializes this object's state vector
 * @param time  the time associated with this state
 */
StateVector::StateVector(const std::vector<double> &state,
                         double time)
: m_state(state),
  m_time(time)
{

}

/**
 * Constructor
 * @param matrix initializes this object's state vector
 * @param time   the time associated with this state
 */
StateVector::StateVector(const Matrix2d &matrix,
                         double time)
: m_state(matrix),
  m_time(time)
{

}

/**
 * Copy constructor
 */
StateVector::StateVector(const StateVector &stateVector)
{
    operator = (stateVector);
}

/**
 * Move constructor
 */
StateVector::StateVector(StateVector &&stateVector)
{
    operator = (std::move(stateVector));
}

/**
 * Destructor
 */
StateVector::~StateVector(void)
{

}

/**
 * Copy assignment operator
 */
StateVector &StateVector::operator = (const StateVector &stateVector)
{
    if (&stateVector != this)
    {
        m_state = stateVector.m_state;
        m_time = stateVector.m_time;
    }

    return *this;
}

/**
 * Move assignment operator
 */
StateVector &StateVector::operator = (StateVector &&stateVector)
{
    if (&stateVector != this)
    {
        stateVector.swap(*this);
    }

    return *this;
}

/**
 * Conversion to std::vector
 */
StateVector::operator std::vector<double> & (void)
{
    return m_state;
}

/**
 * Conversion to std::vector
 */
StateVector::operator std::vector<double> (void) const
{
    return m_state;
}

/**
 * Conversion to Matrix2d
 */
StateVector::operator Matrix2d (void) const
{
    return m_state;
}

/**
 * Subscript operator
 */
double &StateVector::operator [] (int index)
{
    return m_state[index];
}

/**
 * Subscript operator
 */
double StateVector::operator [] (int index) const
{
    return m_state[index];
}

/**
 * begin() overload/
 */
StateVector::iterator StateVector::begin(void)
{
    return m_state.begin();
}

/**
 * cbegin() overload
 */
StateVector::const_iterator StateVector::cbegin(void) const
{
    return m_state.cbegin();
}

/**
 * cend() overload
 */
StateVector::const_iterator StateVector::cend(void) const
{
    return m_state.cend();
}

/**
 * Clear this state vector of its entries
 */
void StateVector::clear(void)
{
    m_state.clear();
}

/**
 * clone() function
 */
StateVector *StateVector::clone(void) const
{
    return new StateVector(*this);
}

/**
 * crbegin() overload
 */
StateVector::const_reverse_iterator StateVector::crbegin(void) const
{
    return m_state.crbegin();
}

/**
 * crend() overload
 */
StateVector::const_reverse_iterator StateVector::crend(void) const
{
    return m_state.crend();
}

/**
 * Function to deserialize this object's data
 */
std::istream &StateVector::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_time, sizeof(double));

        std::size_t size = 0;
        stream.read((char *)&size, sizeof(std::size_t));

        m_state.resize(size);
        for (std::size_t i = 0; stream && i < size; ++i)
        {
            stream.read((char *)&m_state[i], sizeof(double));
        }
    }

    return stream;
}

/**
 * Test for empty object
 */
bool StateVector::empty(void) const
{
    return m_state.empty();
}

/**
 * end() overload
 */
StateVector::iterator StateVector::end(void)
{
    return m_state.end();
}

/**
 * Erase the entry in this state vector associated with the input iterator
 */
StateVector::iterator StateVector::erase(iterator it)
{
    return m_state.erase(it);
}

/**
 * Get the name of this class
 */
std::string StateVector::getClassName(void) const
{
    return "StateVector";
}

/**
 * Get the time associated with this state vector
 */
double StateVector::getTime(void) const
{
    return m_time;
}

/**
 * Initialization function
 */
bool StateVector::initialize(void)
{
    m_time = 0.0;

    return true;
}

/**
 * Output stream print function
 * @param stream a reference to an std::ostream object
 */
std::ostream &StateVector::print(std::ostream &stream) const
{
    if (stream)
    {
        stream << "Time: " << m_time << std::endl;
        stream << "State Vector:" << std::endl;
        for (std::size_t i = 0; i < m_state.size(); ++i)
        {
            stream << "x[" << i << "] = " << m_state[i] << std::endl;
        }
    }

    return stream;
}

/**
 * rbegin() overload
 */
StateVector::reverse_iterator StateVector::rbegin(void)
{
    return m_state.rbegin();
}

/**
 * rend() overload
 */
StateVector::reverse_iterator StateVector::rend(void)
{
    return m_state.rend();
}

/**
 * Re-size this state vector
 */
void StateVector::resize(std::size_t size)
{
    m_state.resize(size, 0.0);
}

/**
 * Function to serialize this object's data
 */
std::ostream &StateVector::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_time, sizeof(double));

        auto size = m_state.size();
        stream.write((const char *)&size, sizeof(std::size_t));
        for (std::size_t i = 0; i < size; ++i)
        {
            stream.write((const char *)&m_state[i], sizeof(double));
        }
    }

    return stream;
}

/**
 * Function to this object's state vector
 */
void StateVector::set(const std::vector<double> &state)
{
    m_state = state;
}

/**
 * Set the time associated with this state vector
 */
void StateVector::setTime(double time)
{
    m_time = time;
}

/**
 * Return the size of this state vector
 */
std::size_t StateVector::size(void) const
{
    return m_state.size();
}

/**
 * Swap function
 */
void StateVector::swap(StateVector &stateVector)
{
    m_state.swap(stateVector.m_state);
    std::swap(m_time, stateVector.m_time);
}

}

}
