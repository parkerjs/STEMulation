#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "arithmetic_attributes.h"
#include "complex.h"
#include "jenkins_traub.h"
#include "output_streamable.h"
#include "reflective.h"
#include "sequence_convolver.h"
#include <functional>
#include <iomanip>

namespace math
{

namespace expression
{

namespace polynomial
{

/**
 * This class represents a polynomial expression and contains methods to perform operations thereupon.
 */
template<typename T>
class Polynomial final
: public attributes::operators::arithmetic::ElementaryArithmeticAttributes<Polynomial<T>>,
  public attributes::operators::arithmetic::Modulable<Polynomial<T>>,
  public attributes::concrete::OutputStreamable<Polynomial<T>>,
  virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticMutexMappable<Polynomial<T>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Polynomial<T>>
{
public:

    /**
     * Constructor
     * @param degree the degree of the polynomial to be constructed
     */
    Polynomial(std::size_t degree = 0)
    : m_coeff(degree + 1, 0)
    {

    }

    /**
     * Constructor
     * @param coeff a vector of coefficients. For example, { 1, 0, 2 } specifies the polynomial x^2+2 { 4, 0,
     *              7, 3, 0 } specifies the polynomial 4x^4+7x^2+3x
     */
    Polynomial(const std::vector<T> &coeff)
    : m_coeff(coeff)
    {
        if (coeff.empty())
        {
            m_coeff.resize(1, 0);

            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "Coefficient vector cannot be empty; constructing a polynomial of degree 0."
                      << std::endl << std::endl;
            this->unlock(0);
        }
    }

    /**
     * Copy constructor
     */
    Polynomial(const Polynomial<T> &polynomial)
    {
        operator = (polynomial);
    }

    /**
     * Move constructor
     */
    Polynomial(Polynomial<T> &&polynomial)
    {
        operator = (std::move(polynomial));
    }

    /**
     * Destructor
     */
    virtual ~Polynomial(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Polynomial<T> &operator = (const Polynomial<T> &polynomial)
    {
        if (&polynomial != this)
        {
            m_coeff = polynomial.m_coeff;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Polynomial<T> &operator = (Polynomial<T> &&polynomial)
    {
        if (&polynomial != this)
        {
            m_coeff = std::move(polynomial.m_coeff);
        }

        return *this;
    }

    /**
     * Subscript operator
     */
    inline virtual T &operator [] (int index) final
    {
        return m_coeff[index];
    }

    /**
     * Subscript operator
     */
    inline virtual T operator [] (int index) const final
    {
        return m_coeff[index];
    }

    /**
     * Equality operator overload
     */
    inline virtual bool operator == (const Polynomial<T> &polynomial) const final
    {
        return m_coeff == polynomial.m_coeff;
    }

    /**
     * Inequality operator overload
     */
    inline virtual bool operator != (const Polynomial<T> &polynomial) const final
    {
        return m_coeff != polynomial.m_coeff;
    }

    /**
     * Additive inverse operator
     */
    inline virtual Polynomial<T> &operator - (void) final
    {
        std::for_each(m_coeff.begin(), m_coeff.end(), std::negate<T>());

        return *this;
    }

    /**
     * Additive inverse operator (const version)
     */
    inline virtual Polynomial<T> operator - (void) const final
    {
        Polynomial<T> poly(*this);
        std::for_each(poly.m_coeff.begin(), poly.m_coeff.end(), std::negate<T>());

        return poly;
    }

    /**
     * Compute remainder resulting from polynomial division and assign to current object
     */
    inline virtual Polynomial<T> &operator %= (const Polynomial<T> &polynomial) final
    {
        std::vector<T> quotient;
        math::signal_processing::SequenceConvolver<T> sequenceConvolver;
        sequenceConvolver.deconvolve(polynomial.m_coeff.cbegin(), polynomial.m_coeff.cend(), m_coeff.cbegin(),
                                     m_coeff.cend(), quotient, m_coeff);

        return *this;
    }

    /**
     * Addition-assignment operator overload
     */
    inline virtual Polynomial<T> &operator += (const Polynomial<T> &polynomial) final
    {
        return addAssign(polynomial);
    }

    /**
     * Subtraction-assignment operator overload
     */
    inline virtual Polynomial<T> &operator -= (const Polynomial<T> &polynomial) final
    {
        return subtractAssign(polynomial);
    }

    /**
     * Multiplication-assignment operator overload
     */
    inline virtual Polynomial<T> &operator *= (const Polynomial<T> &polynomial) final
    {
        return multiplyAssign(polynomial);
    }

    /**
     * Division-assignment operator overload
     */
    inline virtual Polynomial<T> &operator /= (const Polynomial<T> &polynomial) final
    {
        return divideAssign(polynomial);
    }

    /**
     * Add a polynomial to the current object and return result
     */
    inline virtual Polynomial<T> add(const Polynomial<T> &polynomial) const final
    {
        auto n = m_coeff.size();
        auto m = polynomial.m_coeff.size();
        Polynomial<T> result(std::max(m, n) - 1);

        add(result, *this, polynomial);

        return result;
    }

    /**
     * Add the two polynomials and return result
     * @param[in]  result  the exact storage necessary to hold the sum of the polynomials
     * @param      polyOne the LHS polynomial
     * @param      polyTwo the RHS polynomial
     * @param[out] result  if successful, contains the sum of the polynomials
     */
    inline static bool add(Polynomial<T> &result, const Polynomial<T> &polyOne, const Polynomial<T> &polyTwo)
    {
        return sum(result, polyOne, polyTwo);
    }

    /**
     * Add a polynomial to the current object and assign to current object
     */
    inline virtual Polynomial<T> &addAssign(const Polynomial<T> &polynomial) final
    {
        auto n = m_coeff.size();
        auto m = polynomial.m_coeff.size();
        if (n < m)
            m_coeff.insert(m_coeff.begin(), m - n, 0);

        add(*this, *this, polynomial);

        return *this;
    }

    /**
     * Return the anti-derivative of the current object's polynomial
     * @param c a constant of integration
     */
    virtual Polynomial<T> antiDerivative(const T &c) const final
    {
        // find the first non-zero coefficient
        std::size_t j = 0;
        auto n = m_coeff.size();
        while (j < n && m_coeff[j] == 0.0)
            ++j;

        Polynomial<T> antiDerivative(n - j);
        std::copy(m_coeff.cbegin() + j, m_coeff.cend(), antiDerivative.m_coeff.begin());

        antiDerivative.m_coeff.back() = c;

        auto m = antiDerivative.m_coeff.size();
        if (m > 1)
        {
            auto mm1 = m - 1;
            auto mm2 = mm1 - 1;
            for (std::size_t i = 0; i < mm2; ++i)
                antiDerivative.m_coeff[i] /= (mm1 - i);
        }

        return antiDerivative;
    }

    /**
     * Anti-differentiate the current object's polynomial and assign to the current object
     * @param c a constant of integration
     */
    virtual Polynomial<T> &antiDifferentiate(const T &c) final
    {
        // find the first non-zero coefficient
        std::size_t j = 0;
        auto n = m_coeff.size();
        while (j < n && m_coeff[j] == 0.0)
            ++j;

        m_coeff.erase(m_coeff.begin(), m_coeff.begin() + j);
        m_coeff.push_back(c);

        auto m = m_coeff.size();
        if (m > 1)
        {
            auto mm1 = m - 1;
            auto mm2 = mm1 - 1;
            for (std::size_t i = 0; i < mm2; ++i)
                m_coeff[i] /= (mm1 - i);
        }

        return *this;
    }

    /**
     * Return the derivative of the current object's polynomial
     */
    virtual Polynomial<T> derivative(void) const final
    {
        // find the first non-zero coefficient, but don't count the lowest order coefficient, even if zero
        std::size_t j = 0;
        auto n = m_coeff.size();
        while (j + 1 < n && m_coeff[j] == 0.0)
            ++j;

        auto p = n - j - 1;
        Polynomial<T> derivative;
        derivative.m_coeff.resize(p > 0 ? p : 1);

        if (p == 0)
            derivative.m_coeff[0] = 0.0;
        else
        {
            std::copy(m_coeff.cbegin() + j, m_coeff.cend() - 1, derivative.m_coeff.begin());

            auto m = derivative.m_coeff.size();
            auto mm1 = m - 1;
            for (std::size_t i = 0; i < mm1; ++i)
                derivative.m_coeff[i] *= m - i;
        }

        return derivative;
    }

    /**
     * Differentiate the current object's polynomial and assign to the current object
     */
    virtual Polynomial<T> &differentiate(void) final
    {
        // find the first non-zero coefficient, but don't
        // count the lowest order coefficient, even if zero
        std::size_t j = 0;
        auto n = m_coeff.size();
        while (j + 1 < n && m_coeff[j] == 0.0)
            ++j;

        m_coeff.erase(m_coeff.begin(), m_coeff.begin() + j);
        if (m_coeff.size() > 1)
        {
            m_coeff.pop_back();

            auto m = m_coeff.size();
            auto mm1 = m - 1;
            for (std::size_t i = 0; i < mm1; ++i)
                m_coeff[i] *= m - i;
        }
        else
            m_coeff[0] = 0.0;

        return *this;
    }

    /**
     * Polynomial division
     */
    virtual Polynomial<T> divide(const Polynomial<T> &polynomial) const final
    {
        Polynomial<T> quotient(*this);

        return quotient.divideAssign(polynomial);
    }

    /**
     * Divide current object by a polynomial and assign to current object
     */
    virtual Polynomial<T> &divideAssign(const Polynomial<T> &polynomial) final
    {
        std::vector<T> remainder;
        math::signal_processing::SequenceConvolver<T> sequenceConvolver;
        sequenceConvolver.deconvolve(polynomial.m_coeff.cbegin(), polynomial.m_coeff.cend(), m_coeff.cbegin(),
                                     m_coeff.cend(), m_coeff, remainder);

        return *this;
    }

    /**
     * Evaluate the polynomial at the specified abscissa using Horner's method
     */
    inline virtual T evaluate(const T &x) const final
    {
        auto y = m_coeff[0];
        auto n = m_coeff.size();
        for (std::size_t i = 1; i < n; ++i)
            y = m_coeff[i] + x * y;

        return y;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Polynomial";
    }

    /**
     * Get the coefficient vector
     */
    inline virtual void getCoefficients(std::vector<T> &coeff) const final
    {
        auto n = m_coeff.size();
        if (coeff.size() != n)
            coeff.resize(n);

        std::copy(m_coeff.cbegin(), m_coeff.cend(), coeff.begin());
    }

    /**
     * Get the coefficient vector
     */
    inline virtual std::vector<T> getCoefficients(void) const final
    {
        return m_coeff;
    }

    /**
     * Get the order of the polynomial
     */
    inline virtual std::size_t getOrder(void) const final
    {
        return m_coeff.size() - 1;
    }

    /**
     * Multiply current object's polynomial by another polynomial and return result
     */
    inline virtual Polynomial<T> multiply(const Polynomial<T> &polynomial) const final
    {
        auto n = m_coeff.size();
        auto m = polynomial.m_coeff.size();
        Polynomial<T> result(n + m - 2);

        multiply(result, *this, polynomial);

        return result;
    }

    /**
     * Multiply two polynomials
     */
    static bool multiply(Polynomial<T> &result, const Polynomial<T> &polyOne, const Polynomial<T> &polyTwo)
    {
        auto sizeOne = polyOne.m_coeff.size();
        auto sizeTwo = polyTwo.m_coeff.size();
        auto sizeResult = std::max(sizeOne, sizeTwo);

        if (&result != &polyOne && &result != &polyTwo && 1 + result.m_coeff.size() >= sizeResult)
        {
            math::signal_processing::SequenceConvolver<T> sequenceConvolver;

            return sequenceConvolver.convolve(polyOne.m_coeff.cbegin(), polyOne.m_coeff.cend(),
                                              polyTwo.m_coeff.cbegin(), polyTwo.m_coeff.cend(),
                                              result.m_coeff);
        }

        attributes::concrete::StaticSynchronizable<Polynomial<T>>::lock(0);
        std::cout << "Warning from " << getQualifiedStaticMethodName(__func__, Polynomial<T>) << ": "
                  << "Multiplication failed." << std::endl << std::endl;
        attributes::concrete::StaticSynchronizable<Polynomial<T>>::unlock(0);

        return false;
    }

    /**
     * Multiply current object's polynomial by another polynomial and assign to current object
     */
    virtual Polynomial<T> &multiplyAssign(const Polynomial<T> &polynomial) final
    {
        auto n = m_coeff.size();
        auto m = polynomial.m_coeff.size();
        Polynomial<T> result(n + m - 2);

        bool bSuccess = multiply(result, *this, polynomial);
        if (bSuccess)
        {
            m_coeff.resize(m + n - 1);
            std::copy(result.m_coeff.cbegin(), result.m_coeff.cend(), m_coeff.begin());
        }

        return *this;
    }

protected:

    /**
     * Print wrapper for polynomials with real coefficients
     */
    template<typename U>
    struct PolynomialPrinter final
    {
        /**
         * Print function
         */
        virtual std::ostream &print(std::ostream &stream, const Polynomial<U> &poly) const final
        {
            auto prec = stream.precision();
            auto width = stream.width();

            stream.width(0);

            auto n = poly.m_coeff.size();
            auto nm1 = n - 1;

            // find the first non-zero coefficient, but don't
            // count the lowest order coefficient, even if zero
            std::size_t j = 0;
            while (poly.m_coeff[j] == 0.0 && j < nm1)
                ++j;

            for (std::size_t i = j; i < n; ++i)
            {
                auto &&coeff = poly.m_coeff[i];
                if (coeff != 0.0)
                {
                    stream << (i == j ? (coeff < 0.0 ? "-" : "") : (coeff < 0.0 ? " - ": " + "));

                    if (std::abs(coeff) != 1.0)
                        stream << std::setw(width) << std::setprecision(prec) << std::abs(coeff);

                    if (i < nm1)
                    {
                        stream << "x";
                        if (i + 1 < nm1)
                            stream << "^" << nm1 - i;
                    }
                }
                else if (j == nm1)
                    stream << std::setw(width) << std::setprecision(prec) << 0.0;
            }

            stream.width(width);

            return stream;
        }
    };

    /**
     * Print wrapper specialization for polynomials with complex coefficients
     */
    template<typename U>
    struct PolynomialPrinter<std::complex<U>> final
    {
        /**
         * Print function
         */
        virtual std::ostream &print(std::ostream &stream, const Polynomial<std::complex<U>> &poly) const final
        {
            std::streamsize prec = stream.precision();
            std::streamsize width = stream.width();
            std::stringstream oss;

            stream.width(0);

            auto n = poly.m_coeff.size();
            auto nm1 = n - 1;

            // find the first non-zero coefficient, but don't
            // count the lowest order coefficient, even if zero
            std::size_t j = 0;
            while (poly.m_coeff[j] == 0.0 && j < nm1)
                ++j;

            for (std::size_t i = j; i < n; ++i)
            {
                auto &&coeff = poly.m_coeff[i];
                auto &&re = coeff.real();
                auto &&im = coeff.imag();

                bool bNonZeroImag = im != 0.0;
                bool bNonZeroReal = re != 0.0;
                bool bBothNonZero = bNonZeroImag && bNonZeroReal;
                bool bBothZero = !bNonZeroImag && !bNonZeroReal;
                if (bBothNonZero)
                {
                    if (re < 0.0)
                        oss << (i == nm1 ? (i == j ? "-" : " - ") : (i == j ? "-(" : " - ("))
                            << std::setprecision(prec) << -re << (im < 0.0 ? " + j" : " - j")
                            << std::setprecision(prec) << std::abs(im) << (i == nm1 ? "" : ")");
                    else
                        oss << (i == nm1 ? (i == j ? "" : " + ") : (i == j ? "(" : " + ("))
                            << number_systems::complex::Complex<U>(coeff) << (i == nm1 ? "" : ")");
                }
                else if (bNonZeroReal)
                {
                    if (re < 0.0)
                        oss << (i == j ? "-" : " - ") << std::setprecision(prec) << -re;
                    else
                        oss << (i == j ? "" : " + ") << std::setprecision(prec) << re;
                }
                else if (bNonZeroImag)
                {
                    if (im < 0.0)
                        oss << (i == j ? "-j" : " - j") << std::setprecision(prec) << -im;
                    else
                        oss << (i == j ? "j" : " + j") << std::setprecision(prec) << im;
                }
                else if (j == nm1)
                    oss << std::setprecision(prec) << 0.0;

                stream << std::setw(width) << oss.str();

                if (!bBothZero)
                {
                    oss.clear();
                    oss.str("");

                    if (i < nm1)
                    {
                        stream << "x";

                        if (i + 1 < nm1)
                            stream << "^" << nm1 - i;
                    }
                }
            }

            stream.width(width);

            return stream;
        }
    };

public:

    /**
     * Print function
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        PolynomialPrinter<T> polyPrinter;

        return polyPrinter.print(stream, *this);
    }

    /**
     * Find the roots of the polynomial. Return the number of roots found
     * @param[out] roots a vector with necessary storage to contain the result
     */
    template<typename U>
    inline std::size_t roots(std::vector<std::complex<U>> &roots) const
    {
        solvers::JenkinsTraub<U> rootSolver;

        return rootSolver.findRoots(m_coeff, roots);
    }

    /**
     * Find the roots of the polynomial. Return the number of roots found
     * @param[out] a vector with necessary storage to contain the result
     */
    template<typename U>
    inline std::size_t roots(std::vector<U> &roots) const
    {
        solvers::JenkinsTraub<U> rootSolver;

        return rootSolver.findRoots(m_coeff, roots);
    }

    /**
     * Set the coefficient vector
     */
    virtual bool setCoefficients(const std::vector<T> &coeff) final
    {
        if (!coeff.empty())
        {
            auto n = coeff.size();
            if (m_coeff.size() != n)
                m_coeff.resize(n);

            std::copy(coeff.cbegin(), coeff.cend(), m_coeff.begin());

            return true;
        }

        this->lock(0);
        std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                  << "Coefficient vector cannot be empty." << std::endl << std::endl;
        this->unlock(0);

        return false;
    }

    /**
     * Subtract a polynomial from the current object and return result
     */
    inline virtual Polynomial<T> subtract(const Polynomial<T> &polynomial) const final
    {
        auto n = m_coeff.size();
        auto m = polynomial.m_coeff.size();
        Polynomial<T> result(std::max(m, n) - 1);

        subtract(result, *this, polynomial);

        return result;
    }

    /**
     * Subtract two polynomials
     * @param[in]  result  the exact storage necessary to hold the difference of the polynomials
     * @param      polyOne the LHS polynomial
     * @param      polyTwo the RHS polynomial
     * @param[out] result  if successful, contains the difference of the polynomials
     */
    inline static bool subtract(Polynomial<T> &result, const Polynomial<T> &polyOne,
                                const Polynomial<T> &polyTwo)
    {
        return sum(result, polyOne, polyTwo, true);
    }

    /**
     * Subtract a polynomial from the current object and assign to current object
     */
    inline virtual Polynomial<T> &subtractAssign(const Polynomial<T> &polynomial) final
    {
        auto n = m_coeff.size();
        auto m = polynomial.m_coeff.size();
        if (n < m)
            m_coeff.insert(m_coeff.begin(), m - n, 0);

        subtract(*this, *this, polynomial);

        return *this;
    }

protected:

    /**
     * Sum the two polynomials and return result
     * @param[in]  result           the exact storage necessary to hold the sum of the polynomials
     * @param      polyOne          the LHS polynomial
     * @param      polyTwo          the RHS polynomial
     * @param      bAdditiveInverse flag indicating subtraction (true) or addition (false)
     * @param[out] result           if successful, contains the sum of the polynomials
     */
    static bool sum(Polynomial<T> &result, const Polynomial<T> &polyOne, const Polynomial<T> &polyTwo,
                    bool bAdditiveInverse = false)
    {
        auto sizeOne = polyOne.m_coeff.size();
        auto sizeTwo = polyTwo.m_coeff.size();
        auto sizeResult = result.m_coeff.size();
        auto maxSize = std::max(sizeOne, sizeTwo);

        if (sizeResult >= maxSize)
        {
            if (sizeResult > maxSize)
                std::fill(result.m_coeff.begin(), result.m_coeff.end() - maxSize, 0);

            if (sizeTwo > sizeOne)
            {
                std::copy(polyTwo.m_coeff.cbegin(), polyTwo.m_coeff.cend() - sizeOne,
                          result.m_coeff.end() - sizeTwo);

                if (bAdditiveInverse)
                {
                    std::for_each(result.m_coeff.end() - sizeTwo, result.m_coeff.end() - sizeOne,
                                  std::negate<T>());
                    std::transform(polyOne.m_coeff.cbegin(), polyOne.m_coeff.cend(),
                                   polyTwo.m_coeff.cend() - sizeOne, result.m_coeff.end() - sizeOne,
                                   std::minus<T>());
                }
                else
                    std::transform(polyOne.m_coeff.cbegin(), polyOne.m_coeff.cend(),
                                   polyTwo.m_coeff.cend() - sizeOne, result.m_coeff.end() - sizeOne,
                                   std::plus<T>());
            }
            else
            {
                std::copy(polyOne.m_coeff.cbegin(), polyOne.m_coeff.cend(), result.m_coeff.end() - sizeOne);

                if (bAdditiveInverse)
                    std::transform(result.m_coeff.cend() - sizeTwo,
                                   result.m_coeff.cend(), polyTwo.m_coeff.cbegin(),
                                   result.m_coeff.end() - sizeTwo, std::minus<T>());
                else
                    std::transform(result.m_coeff.cend() - sizeTwo,
                                   result.m_coeff.cend(), polyTwo.m_coeff.cbegin(),
                                   result.m_coeff.end() - sizeTwo, std::plus<T>());
            }

            return true;
        }

        attributes::concrete::StaticSynchronizable<Polynomial<T>>::lock(0);
        std::cout << "Warning from " + getQualifiedStaticMethodName(__func__, Polynomial<T>) + ": "
                  << "Summation failed." << std::endl << std::endl;
        attributes::concrete::StaticSynchronizable<Polynomial<T>>::unlock(0);

        return false;
    }

    /**
     * coefficient vector
     */
    std::vector<T> m_coeff;
};

}

}

}

#endif
