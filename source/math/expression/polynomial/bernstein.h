#ifndef BERNSTEIN_POLYNOMIAL_H
#define BERNSTEIN_POLYNOMIAL_H

#include "algorithm.h"
#include "cloneable.h"
#include "factorial.h"
#include "reflective.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <cmath>
#include <iostream>
#include <mutex>

namespace math
{

namespace expression
{

namespace polynomial
{

/**
 * This class represents a Bernstein polynomial
 */
template<typename T, size_t N>
class BernsteinPolynomial final
: public attributes::interfaces::Cloneable<BernsteinPolynomial<T, N>>,
  virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticMutexMappable<BernsteinPolynomial<T, N>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<BernsteinPolynomial<T, N>>
{
public:

    /**
     * Constructor
     */
    BernsteinPolynomial(void)
    {

    }

    /**
     * Copy constructor
     */
    BernsteinPolynomial(const BernsteinPolynomial<T, N> &polynomial)
    {
        operator = (polynomial);
    }

    /**
     * Move constructor
     */
    BernsteinPolynomial(BernsteinPolynomial<T, N> &&polynomial)
    {
        operator = (std::move(polynomial));
    }

    /**
     * Destructor
     */
    virtual ~BernsteinPolynomial(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    BernsteinPolynomial<T, N> &operator = (const BernsteinPolynomial<T, N> &polynomial)
    {
        if (&polynomial != this)
        {
            // TODO: add variables here.
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    BernsteinPolynomial<T, N> &operator = (BernsteinPolynomial<T, N> &&polynomial)
    {
        if (&polynomial != this)
        {
            // TODO: add variables here.
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual BernsteinPolynomial<T, N> *clone(void) const override
    {
        return new BernsteinPolynomial<T, N>(*this);
    }

    /**
     * Evaluate the ith Bernstein basis polynomial at t, where 0 <= t <= 1.0
     */
    inline virtual T evaluate(size_t i, T t) final
    {
        if (t < 0.0 || t > 1.0)
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "t must lie within the closed interval [0.0, 1.0]" << std::endl
                      << "Message from " + getQualifiedMethodName(__func__) + ": "
                      << "Resetting t to " << (t < 0.0 ? "0.0" : "1.0") << std::endl;
            this->unlock(0);
        }

        // prevent problems with std::pow
        auto &&ti = (t == 0.0 && i == 0) ? 1.0 : std::pow(t, i); // t^i
        auto &&tni = (N == i && t == 1.0) ? 1.0 : std::pow(1.0 - t, N - i); // (1 - t)^i

        auto &&factorials = combinatorics::FactorialTable<N>::getData();
        auto &&ni =  factorials[N] / (factorials[i] * factorials[N - i]);

        return ni * ti * tni;
    }

    /**
     * Evaluate the Bernstein polynomial at t, where 0 <= t <= 1.0
     */
    inline virtual T evaluate(const T &t) final
    {
        T b_n = (T)0;
        for (size_t i = 0; i <= N; ++i)
            b_n += evaluate(i, t);

        return b_n;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Bernstein";
    }
};

}

}

}

#endif
