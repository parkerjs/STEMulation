#ifndef EXPRESSION_FUNCTOR_H
#define EXPRESSION_FUNCTOR_H

#include "any.h"
#include "any_configurable.h"
#include "dictionary.h"
#include "factory_constructible.h"
#include "functor.h"
#include "nameable.h"
#include "setupable.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"

namespace utilities
{

namespace expression
{

/**
 * Base class for expression function objects
 */
template<typename T>
class ExpressionFunctor
: public attributes::interfaces::AnyConfigurable,
  public attributes::abstract::FactoryConstructible<ExpressionFunctor<T>>,
  public functional::functors::Functor<functional::Any>,
  public attributes::interfaces::Nameable,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::concrete::VariableMapRegistrable
{
protected:

    /**
     * Constructor
     * @param name the name of the function associated with this class
     */
    ExpressionFunctor(const std::string &name)
    : m_name(name)
    {

    }

    /**
     * Copy constructor
     */
    ExpressionFunctor(const ExpressionFunctor &functor)
    {
        operator = (functor);
    }

    /**
     * Move constructor
     */
    ExpressionFunctor(ExpressionFunctor &&functor)
    {
        operator = (std::move(functor));
    }

public:

    /**
     * Destructor
     */
    virtual ~ExpressionFunctor(void) override
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    ExpressionFunctor &operator = (const ExpressionFunctor &functor)
    {
        if (&functor != this)
        {
            m_name = functor.m_name;
            m_result = functor.m_result;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    ExpressionFunctor &operator = (ExpressionFunctor &&functor)
    {
        if (&functor != this)
        {
            m_name = std::move(functor.m_name);
            m_result = std::move(functor.m_result);
        }

        return *this;
    }

public:

    /**
     * Function call operator
     */
    virtual functional::Any operator () (std::vector<functional::Any> &arguments) = 0;

    /**
     * Configure function (using a vector of Any objects)
     */
    inline virtual bool configure(const std::vector<functional::Any> &anys) override
    {
        return true; // TODO: determine if this may be implemented by derived classes?
    }

    /**
     * Configure function (using a variable token map)
     */
    inline virtual bool configure(tTokenMap &tokenMap) override
    {
        utilities::Dictionary dictionary(&m_registry);

        return dictionary.populate(tokenMap);
    }

    /**
     * create() function
     * @param type the kind of expression functor to be instantiated
     */
    inline static ExpressionFunctor<T> *create(const std::string &type)
    {
        return attributes::abstract::FactoryConstructible<ExpressionFunctor<T>>::create(type);
    }

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override
    {
        return getName();
    }

    /**
     * Get the name assigned to this object
     */
    inline virtual std::string getName(void) const override final
    {
        return m_name;
    }

    /**
     * Get this function's parameter signature
     */
    inline virtual std::string getParameterSignature(void) const
    {
        return "()";
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        return stream;
    }

    /**
     * Set the name assigned to this object
     */
    inline virtual void setName(const std::string &name) override
    {
        m_name = name;
    }

    /**
     * Setup function
     */
    inline virtual bool setup(void) override
    {
        // setup variable registry configurable variables
        m_registry["name"] = m_name;

        return true;
    }

protected:

    /**
     * name of the function associated with this class
     */
    std::string m_name;

    /**
     * workspace result vector
     */
    std::vector<T> m_result;
};

}

}

#endif
