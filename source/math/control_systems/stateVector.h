#ifndef STATE_VECTOR_H
#define STATE_VECTOR_H

#include "cloneable.h"
#include "export_library.h"
#include "inherited_iterator.h"
#include "initializable.h"
#include "iterable.h"
#include "output_streamable.h"
#include "reflective.h"
#include "reverse_iterable.h"
#include "serializable.h"
#include "swappable.h"
#include <vector>

namespace math
{

// forward declarations
namespace linear_algebra { namespace matrix { class Matrix2d; } }

namespace control_systems
{

/**
 * This class represents a state vector in a control system
 */
class StateVector
: public attributes::interfaces::Cloneable<StateVector>,
  public attributes::interfaces::Initializable,
  public attributes::abstract::Iterable<iterators::Iterator, std::vector<double>,
                                        iterators::inherited_iterator_tag>,
  public attributes::concrete::OutputStreamable<StateVector>,
  virtual private attributes::abstract::Reflective,
  public attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<double>,
                                               iterators::inherited_iterator_tag>,
  public attributes::interfaces::Serializable,
  public attributes::interfaces::Swappable<StateVector>
{
public:

    /**
     * Type alias declarations
     */
    using Matrix2d = math::linear_algebra::matrix::Matrix2d;

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, StateVector, iterators
                      ::inherited_iterator_tag>::const_iterator const_iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, StateVector, iterators
                      ::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, StateVector, iterators
                      ::inherited_iterator_tag>::iterator iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, StateVector, iterators
                      ::inherited_iterator_tag>::reverse_iterator reverse_iterator;

    /**
     * Constructor
     * @param size the size of the state vector
     * @param time the time associated with this state
     */
    EXPORT_STEM StateVector(std::size_t size = 1,
                            double time = 0.0);

    /**
     * Constructor
     * @param state initializes this object's state vector
     * @param time  the time associated with this state
     */
    EXPORT_STEM StateVector(const std::vector<double> &state,
                            double time = 0.0);

    /**
     * Constructor
     * @param state initializes this object's state vector
     * @param time  the time associated with this state
     */
    EXPORT_STEM StateVector(const Matrix2d &matrix,
                            double time = 0.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM StateVector(const StateVector &stateVector);

    /**
     * Move constructor
     */
    EXPORT_STEM StateVector(StateVector &&stateVector);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~StateVector(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM StateVector &operator = (const StateVector &stateVector);

    /**
     * Move assignment operator
     */
    EXPORT_STEM StateVector &operator = (StateVector &&stateVector);

    /**
     * Conversion to std::vector
     */
    EXPORT_STEM virtual operator std::vector<double> & (void) final;

    /**
     * Conversion to std::vector
     */
    EXPORT_STEM virtual operator std::vector<double> (void) const final;

    /**
     * Conversion to Matrix2d
     */
    EXPORT_STEM virtual operator Matrix2d (void) const final;

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual double &operator [] (int index) final;

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual double operator [] (int index) const final;

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
     * Clear this state vector of its entries
     */
    EXPORT_STEM virtual void clear(void) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual StateVector *clone(void) const override;

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
     * Test for empty object
     */
    EXPORT_STEM virtual bool empty(void) const final;

    /**
     * end() overload
     */
    EXPORT_STEM virtual iterator end(void) override;

    /**
     * Erase the entry in this state vector associated with the input iterator
     */
    EXPORT_STEM virtual iterator erase(iterator it) final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the time associated with this state vector
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
     * Re-size this state vector
     */
    EXPORT_STEM virtual void resize(std::size_t size);

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Function to this object's state vector
     */
    EXPORT_STEM virtual void set(const std::vector<double> &state);

    /**
     * Set the time associated with this state vector
     */
    EXPORT_STEM virtual void setTime(double time) final;

    /**
     * Return the size of this state vector
     */
    EXPORT_STEM virtual std::size_t size(void) const final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(StateVector &stateVector) override final;

protected:

    /**
     * the state vector
     */
    std::vector<double> m_state;

    /**
     * the time associated with this state vector
     */
    double m_time;
};

}

}

#endif
