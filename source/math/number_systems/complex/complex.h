#ifndef COMPLEX_H
#define COMPLEX_H

#include "arithmetic_attributes.h"
#include "streamable.h"
#include <cmath>
#include <complex>
#include <iomanip>

namespace math
{

namespace number_systems
{

namespace complex
{

/**
 * A class for representing complex numbers
 */
template<typename T>
class Complex
: public attributes::operators::arithmetic::ElementaryArithmeticAttributes<Complex<T>>,
  public attributes::concrete::Streamable<Complex<T>>
{
public:

    /**
     * Enumerations
     */
    enum CoordinateSystemType { Cartesian, // Cartesian coordinate system
                                Polar };   // Polar coordinate system

    /**
     * Constructor
     */
    Complex(void)
    : m_imaginary(0),
      m_real(0)
    {

    }

    /**
     * Constructor
     * @param a, b   represent the real and imaginary components of a complex number for Cartesian systems, or
     *               the magnitude and phase in radians for polar systems
     * @param system the coordinate system type, Cartesian or Polar
     */
    Complex(const T &a, const T &b, CoordinateSystemType system = CoordinateSystemType::Cartesian)
    {
        switch (system)
        {
            case CoordinateSystemType::Cartesian:
            m_imaginary = b;
            m_real = a;
            break;

            case CoordinateSystemType::Polar:
            m_imaginary = a * std::sin(b);
            m_real = a * std::cos(b);
        }
    }

    /**
     * Constructor
     */
    Complex(const T &real)
    : m_imaginary(0),
      m_real(real)
    {

    }

    /**
     * Constructor
     */
    Complex(const std::complex<T> &number)
    : m_imaginary(number.imag()),
      m_real(number.real())
    {

    }

    /**
     * Copy constructor
     */
    Complex(const Complex<T> &number)
    {
        operator = (number);
    }

    /**
     * Move constructor
     */
    Complex(Complex<T> &&number)
    {
        operator = (std::move(number));
    }

    /**
     * Destructor
     */
    virtual ~Complex(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Complex<T> &operator = (const Complex<T> &number)
    {
        if (&number != this)
        {
            m_imaginary = number.m_imaginary;
            m_real = number.m_real;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Complex<T> &operator = (Complex<T> &&number)
    {
        if (&number != this)
        {
            m_imaginary = std::move(number.m_imaginary);
            m_real = std::move(number.m_real);
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    Complex<T> &operator = (const T &real)
    {
        m_imaginary = 0;
        m_real = real;

        return *this;
    }

    /**
     * Move assignment operator
     */
    Complex<T> &operator = (T &&value)
    {
        m_imaginary = 0;
        m_real = std::move(value);

        return *this;
    }

    /**
     * Conversion to std::complex<T>
     */
    virtual operator std::complex<T> (void) const final
    {
        std::complex<T> result(m_real, m_imaginary);

        return result;
    }

    /**
     * Additive inverse operator
     */
    inline virtual Complex<T> &operator - (void) final
    {
        m_imaginary = -m_imaginary;
        m_real = -m_real;

        return *this;
    }

    /**
     * Additive inverse operator (const version)
     */
    inline virtual Complex<T> operator - (void) const final
    {
        return Complex<T>(-m_real, -m_imaginary);
    }

    /**
     * Equality operator
     */
    inline virtual bool operator == (const Complex<T> &number) const final
    {
        return m_real == number.m_real && m_imaginary == number.m_imaginary;
    }

    /**
     * Inequality operator
     */
    inline virtual bool operator != (const Complex<T> &number) const final
    {
        return m_real != number.m_real || m_imaginary != number.m_imaginary;
    }

    /**
     * Addition assignment operator
     */
    inline virtual Complex<T> &operator += (const Complex<T> &number) final
    {
        m_imaginary += number.m_imaginary;
        m_real += number.m_real;

        return *this;
    }

    /**
     * Subtraction assignment operator
     */
    inline virtual Complex<T> &operator -= (const Complex<T> &number) final
    {
        m_imaginary -= number.m_imaginary;
        m_real -= number.m_real;

        return *this;
    }

    /**
     * Multiplication assignment
     */
    inline virtual Complex<T> &operator *= (const Complex<T> &number) final
    {
        T &&imaginary = m_imaginary * number.m_real + m_real * number.m_imaginary;
        T &&real = m_real * number.m_real - m_imaginary * number.m_imaginary;

        m_imaginary = std::move(imaginary);
        m_real = std::move(real);

        return *this;
    }

    /**
     * Division assignment
     */
    inline virtual Complex<T> &operator /= (const Complex<T> &number) final
    {
        auto d = number.m_real * number.m_real + number.m_imaginary * number.m_imaginary;
        T &&imaginary = (m_imaginary * number.m_real - m_real * number.m_imaginary) / d;
        T &&real = (m_real * number.m_real + m_imaginary * number.m_imaginary) / d;

        m_imaginary = std::move(imaginary);
        m_real = std::move(real);

        return *this;
    }

    /**
     * Equality operator
     */
    inline virtual bool operator == (const T &real) const final
    {
        return m_real == real && m_imaginary == 0;
    }

    /**
     * Inequality operator
     */
    inline virtual bool operator != (const T &real) const final
    {
        return m_real != real || m_imaginary != 0;
    }

    /**
     * Addition assignment
     */
    inline virtual Complex<T> &operator += (const T &real) final
    {
        m_real += real;

        return *this;
    }

    /**
     * Subtraction assignment
     */
    inline virtual Complex<T> &operator -= (const T &real) final
    {
        m_real -= real;

        return *this;
    }

    /**
     * Multiplication assignment
     */
    inline virtual Complex<T> &operator *= (const T &real) final
    {
        m_imaginary *= real;
        m_real *= real;

        return *this;
    }

    /**
     * Division assignment
     */
    inline virtual Complex<T> &operator /= (const T &real) final
    {
        m_imaginary /= real;
        m_real /= real;

        return *this;
    }

    /**
     * Complex conjugate
     */
    inline virtual Complex<T> conjugate(void) const final
    {
        return Complex<T>(m_real, -m_imaginary);
    }

    /**
     * Compute the complex exponential
     */
    inline virtual Complex<T> exp(void) const final
    {
        double mag = std::exp(m_real);
        double real = mag * std::cos(m_imaginary);
        double imaginary = mag * std::sin(m_imaginary);

        return Complex<T>(real, imaginary);
    }

    /**
     * Set the imaginary part
     */
    inline virtual void imag(const T &imaginary) final
    {
        m_imaginary = imaginary;
    }

    /**
     * Return a reference to the imaginary part of complex number
     */
    inline virtual T &imag(void) final
    {
        return m_imaginary;
    }

    /**
     * Return imaginary part of complex number
     */
    inline virtual T imag(void) const final
    {
        return m_imaginary;
    }

    /**
     * Inverse of a complex number
     */
    inline virtual Complex<T> inverse(void) const final
    {
        return Complex<T>(*this).invert();
    }

    /**
     * Compute the inverse of a complex number and assign to current object
     */
    inline virtual Complex<T> &invert(void) final
    {
        auto d = m_real * m_real + m_imaginary * m_imaginary;

        m_imaginary /= -d;
        m_real /= d;

        return *this;
    }

    /**
     * Compute absolute value of complex number
     */
    inline virtual T modulus(void) const final
    {
        return std::sqrt(m_real * m_real + m_imaginary * m_imaginary);
    }

    /**
     * Compute phase of complex number in radians
     */
    inline virtual T phase(void) const final
    {
        return std::atan2(m_imaginary, m_real);
    }

    /**
     * Print function
     */
    virtual std::ostream &print(std::ostream &stream) const override
    {
        auto prec = stream.precision();
        auto width = stream.width();
        std::ostringstream oss;

        bool bNonZeroImag = (m_imaginary != 0.0);
        bool bNonZeroReal = (m_real != 0.0);
        if (bNonZeroReal)
        {
            oss << std::setprecision(prec) << m_real;

            if (bNonZeroImag)
                oss << (m_imaginary < 0 ? " - j" : " + j") << std::setprecision(prec) << std::abs(m_imaginary);
        }
        else if (bNonZeroImag)
            oss << (m_imaginary < 0 ? "-" : "") << std::setprecision(prec) << std::abs(m_imaginary);
        else
            oss << std::setprecision(prec) << 0;

        stream << std::setw(width) << oss.str();

        return stream;
    }

    /**
     * Raise a complex number to a complex power
     */
    inline virtual Complex<T> pow(const Complex<T> &power) const final
    {
        std::complex<T> base(m_real, m_imaginary);
        std::complex<T> exponent(power.m_real, power.m_imaginary);
        auto &&result = std::pow(base, exponent);

        return Complex<T>(result.real(), result.imag());
    }

    /**
     * Set the real part
     */
    inline virtual void real(const T &real) final
    {
        m_real = real;
    }

    /**
     * Return a reference to the real part of complex number
     */
    inline virtual T &real(void) final
    {
        return m_real;
    }

    /**
     * Return real part of complex number
     */
    inline virtual T real(void) const final
    {
        return m_real;
    }

    /**
     * Return the square root of a complex number
     */
    inline virtual Complex<T> sqrt(void) const final
    {
        auto arg = 0.5 * phase();
        auto mag = std::pow(m_real * m_real + m_imaginary * m_imaginary, 0.25);

        return Complex<T>(mag, arg, CoordinateSystemType::Polar);
    }

protected:

    /**
     * the imaginary part
     */
    T m_imaginary;

    /**
     * the real part
     */
    T m_real;
};

}

}

}

#endif
