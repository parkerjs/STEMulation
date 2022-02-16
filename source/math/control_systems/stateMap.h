#ifndef STATE_MAP_H
#define STATE_MAP_H

#include "cloneable.h"
#include "export_library.h"
#include "inherited_iterator.h"
#include "initializable.h"
#include "iterable.h"
#include "nameable.h"
#include "output_streamable.h"
#include "reflective.h"
#include "reverse_iterable.h"
#include "serializable.h"
#include "swappable.h"
#include <map>
#include <vector>

namespace math
{

namespace control_systems
{

// forward declarations
class StateVector;

/**
 * This class represents a map of states associated with a control system
 */
class StateMap
: public attributes::interfaces::Cloneable<StateMap>,
  public attributes::interfaces::Initializable,
  public attributes::abstract::Iterable<iterators::Iterator, std::map<std::string, double>,
                                        iterators::inherited_iterator_tag>,
  public attributes::interfaces::Nameable,
  public attributes::concrete::OutputStreamable<StateMap>,
  virtual private attributes::abstract::Reflective,
  public attributes::abstract::ReverseIterable<iterators::Iterator, std::map<std::string, double>,
                                               iterators::inherited_iterator_tag>,
  public attributes::interfaces::Serializable,
  public attributes::interfaces::Swappable<StateMap>
{
public:

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, StateMap, iterators
                      ::inherited_iterator_tag>::const_iterator const_iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, StateMap, iterators
                      ::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, StateMap, iterators
                      ::inherited_iterator_tag>::iterator iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, StateMap, iterators
                      ::inherited_iterator_tag>::reverse_iterator reverse_iterator;

    /**
     * Constructor
     * @param time             the time associated with this object
     * @param availabilityTime the availability time associated with this object
     */
    EXPORT_STEM StateMap(double time = 0.0,
                         double availabilityTime = 0.0);

    /**
     * Constructor
     * @param name             the name associated with this object
     * @param time             the time associated with this object
     * @param availabilityTime the availability time associated with this object
     */
    EXPORT_STEM StateMap(const std::string &name,
                         double time = 0.0,
                         double availabilityTime = 0.0);

    /**
     * Constructor
     * @param stateMap         a map of states used to initialize this object
     * @param time             the time associated with this object
     * @param availabilityTime the availability time associated with this object
     */
    EXPORT_STEM StateMap(const std::map<std::string, double> &stateMap,
                         double time = 0.0,
                         double availabilityTime = 0.0);

    /**
     * Constructor
     * @param stateMap         a map of states used to initialize this object
     * @param name             the name associated with this object
     * @param time             the time associated with this object
     * @param availabilityTime the availability time associated with this object
     */
    EXPORT_STEM StateMap(const std::map<std::string, double> &stateMap,
                         const std::string &name,
                         double time = 0.0,
                         double availabilityTime = 0.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM StateMap(const StateMap &stateMap);

    /**
     * Move constructor
     */
    EXPORT_STEM StateMap(StateMap &&stateMap);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~StateMap(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM StateMap &operator = (const StateMap &stateMap);

    /**
     * Move assignment operator
     */
    EXPORT_STEM StateMap &operator = (StateMap &&stateMap);

    /**
     * Conversion to std::vector
     */
    EXPORT_STEM virtual operator std::vector<double> (void) const;

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual double &operator [] (const std::string &name);

    /**
     * begin() overload
     */
    EXPORT_STEM virtual iterator begin(void) override;

    /**
     * cbegin() overload
     */
    EXPORT_STEM virtual const_iterator cbegin(void) const override;

    /**
     * cend() overload
     */
    EXPORT_STEM virtual const_iterator cend(void) const override;

    /**
     * Clear this object of its entries
     */
    EXPORT_STEM virtual void clear(void) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual StateMap *clone(void) const override;

    /**
     * Query whether or not this object contains a state associated with the given name
     */
    EXPORT_STEM virtual bool contains(const std::string &name) const final;

    /**
     * crbegin() overload
     */
    EXPORT_STEM virtual const_reverse_iterator crbegin(void) const override;

    /**
     * crend() overload
     */
    EXPORT_STEM virtual const_reverse_iterator crend(void) const override;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Test for empty variable registry
     */
    EXPORT_STEM virtual bool empty(void) const final;

    /**
     * end() overload
     */
    EXPORT_STEM virtual iterator end(void) override;

    /**
     * Erase within this object the entry associated with the input iterator
     */
    EXPORT_STEM virtual iterator erase(iterator itStateMap) final;

    /**
     * Retrieve the state associated with the given name; returns true upon success
     */
    EXPORT_STEM virtual bool get(const std::string &name,
                                 double &state) const;

    /**
     * Get the availability time associated with this object
     */
    EXPORT_STEM virtual double getAvailabilityTime(void) const;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the name associated with this object
     */
    EXPORT_STEM std::string getName(void) const override;

    /**
     * Get the time associated with this object
     */
    EXPORT_STEM virtual double getTime(void) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * rbegin() overload
     */
    EXPORT_STEM virtual reverse_iterator rbegin(void) override;

    /**
     * rend() overload
     */
    EXPORT_STEM virtual reverse_iterator rend(void) override;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set the state data associated with the specified name; if an entry doesn't exist, one is created
     */
    EXPORT_STEM virtual bool set(const std::string &name,
                                 double state);

    /**
     * Set the availability time associated with this object
     */
    EXPORT_STEM virtual void setAvailabilityTime(double availabilityTime) final;

    /**
     * Set the name associated with this object
     */
    EXPORT_STEM void setName(const std::string &name) override;

    /**
     * Set the time associated with this object
     */
    EXPORT_STEM virtual void setTime(double time) final;

    /**
     * Return the number of entires in this object
     */
    EXPORT_STEM virtual std::size_t size(void) const final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(StateMap &stateMap) override final;

    /**
     * Function to convert a vector of state map objects into a map of state vectors keyed by name
     * @param  stateMapVector a vector of state map objects
     * @return                a map populated with vectors of states keyed by name
     */
    static EXPORT_STEM std::map<std::string, std::vector<double>>
    toMap(const std::vector<StateMap *> &stateMapVector);

    /**
     * Function to convert a vector of state map objects into a map of state vectors keyed by name
     * @param      stateMapVector a vector of state map objects
     * @param[out] time           a vector of times corresponding to the state map objects
     * @return                    a map populated with vectors of states keyed by name
     */
    static EXPORT_STEM std::map<std::string, std::vector<double>>
    toMap(const std::vector<StateMap *> &stateMapVector,
          std::vector<double> &time);

    /**
     * Function to convert a vector of state map objects into a map of state vectors keyed by name
     * @param      stateMapVector a vector of state map objects
     * @param[out] stateVectorMap populated with vectors of states
     */
    static EXPORT_STEM void toMap(const std::vector<StateMap *> &stateMapVector,
                                  std::map<std::string, std::vector<double>> &stateVectorMap);

    /**
     * Function to convert a vector of state map objects into a map of state vectors keyed by name
     * @param      stateMapVector a vector of state map objects
     * @param[out] stateVectorMap populated with vectors of states
     * @param[out] time           populated with a vector of times corresponding to the state map objects
     */
    static EXPORT_STEM void toMap(const std::vector<StateMap *> &stateMapVector,
                                  std::map<std::string, std::vector<double>> &stateVectorMap,
                                  std::vector<double> &time);

protected:

    /**
     * the availability time associated with this object
     */
    double m_availabilityTime;

    /**
     * the name associated with this object
     */
    std::string m_name;

    /**
     * map of name-state pairs
     */
    std::map<std::string, double> m_stateMap;

    /**
     * the time associated with this object
     */
    double m_time;
};

}

}

#endif
